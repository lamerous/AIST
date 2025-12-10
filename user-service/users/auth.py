from fastapi import APIRouter, Depends, HTTPException, status
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from sqlalchemy.orm import Session
from datetime import datetime, timedelta
from typing import Optional
from jose import JWTError, jwt
import bcrypt

from database import get_db
import models, schemas

import os
from dotenv import load_dotenv

load_dotenv()

SECRET_KEY = os.getenv("JWT_SECRET")
ALGORITHM = "HS256"
ACCESS_TOKEN_EXPIRE_MINUTES = 30

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="/api/auth/login")

router = APIRouter()

def verify_password(plain_password: str, hashed_password: str) -> bool:
    return bcrypt.checkpw(plain_password.encode('utf-8'), hashed_password.encode('utf-8'))

def get_password_hash(password: str) -> str:
    password_bytes = password.encode('utf-8')
    if len(password_bytes) > 72:
        print("PASSWORD MORE THEN 72 BYTES")
        password_bytes = password_bytes[:72]
    
    salt = bcrypt.gensalt()
    hashed = bcrypt.hashpw(password_bytes, salt)
    return hashed.decode('utf-8')

def create_access_token(data: dict, expires_delta: Optional[timedelta] = None):
    to_encode = data.copy()
    if expires_delta:
        expire = datetime.utcnow() + expires_delta
    else:
        expire = datetime.utcnow() + timedelta(minutes=15)
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(to_encode, SECRET_KEY, algorithm=ALGORITHM)
    return encoded_jwt

async def get_current_user(
    token: str = Depends(oauth2_scheme),
    db: Session = Depends(get_db)
):
    credentials_exception = HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Could not validate credentials",
        headers={"WWW-Authenticate": "Bearer"},
    )
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        username: str = payload.get("sub")
        user_id: int = payload.get("user_id")
        role: str = payload.get("role")
        if username is None or user_id is None:
            raise credentials_exception
        token_data = schemas.TokenData(username=username, user_id=user_id, role=role)
    except jwt.PyJWTError:
        raise credentials_exception
    
    user = db.query(models.User).filter(models.User.id == token_data.user_id).first()
    if user is None:
        raise credentials_exception
    return user

async def get_current_active_user(current_user: models.User = Depends(get_current_user)):
    if not current_user:
        raise HTTPException(status_code=400, detail="Inactive user")
    return current_user

async def require_role(required_roles, current_user: models.User = Depends(get_current_user)):
    if not current_user.has_role(required_roles):
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail=f"Requires one of these roles: {required_roles}"
        )
    return current_user

async def require_admin(current_user: models.User = Depends(get_current_user)):
    return await require_role(["admin"], current_user)

async def require_dispatcher(current_user: models.User = Depends(get_current_user)):
    return await require_role(["dispatcher", "admin"], current_user)

async def require_cashier(current_user: models.User = Depends(get_current_user)):
    return await require_role(["cashier", "admin"], current_user)

@router.post("/register", response_model=schemas.UserResponse)
def register(user: schemas.UserCreate, db: Session = Depends(get_db)):
    db_user_by_username = db.query(models.User).filter(
        models.User.username == user.username
    ).first()
    if db_user_by_username:
        raise HTTPException(
            status_code=400,
            detail="Username already registered"
        )
    
    db_user_by_email = db.query(models.User).filter(
        models.User.email == user.email
    ).first()
    if db_user_by_email:
        raise HTTPException(
            status_code=400,
            detail="Email already registered"
        )
        
    hashed_password = get_password_hash(user.password)
    db_user = models.User(
        **user.dict(exclude={"password"}),
        password_hash=hashed_password
    )
    
    db.add(db_user)
    db.commit()
    db.refresh(db_user)
    return db_user

@router.post("/login", response_model=schemas.Token)
def login(form_data: OAuth2PasswordRequestForm = Depends(), db: Session = Depends(get_db)):
    user = db.query(models.User).filter(
        models.User.username == form_data.username
    ).first()
    
    if not user or not verify_password(form_data.password, user.password_hash):
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Incorrect username or password",
            headers={"WWW-Authenticate": "Bearer"},
        )
    
    access_token_expires = timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)
    access_token = create_access_token(
        data={
            "sub": user.username,
            "user_id": user.id,
            "role": user.role
        },
        expires_delta=access_token_expires
    )
    
    return {
        "access_token": access_token,
        "token_type": "bearer",
        "user": user
    }

@router.get("/me", response_model=schemas.UserResponse)
async def read_users_me(current_user: models.User = Depends(get_current_active_user)):
    return current_user

@router.post("/change-password")
async def change_password(
    password_data: schemas.ChangePassword,
    current_user: models.User = Depends(get_current_active_user),
    db: Session = Depends(get_db)
):
    if not verify_password(password_data.current_password, current_user.password_hash):
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Current password is incorrect"
        )
    
    current_user.password_hash = get_password_hash(password_data.new_password)
    db.commit()
    
    return {"message": "Password changed successfully"}