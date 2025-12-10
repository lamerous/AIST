from fastapi import APIRouter, Depends, HTTPException, status, Query
from sqlalchemy.orm import Session
from typing import List, Optional

from database import get_db
import models, schemas
from .auth import get_current_user, require_admin, require_role

router = APIRouter()

@router.get("/", response_model=List[schemas.UserResponse])
def get_users(skip: int = Query(0, ge=0), limit: int = Query(100, ge=1, le=1000), role: Optional[str] = None, search: Optional[str] = None, db: Session = Depends(get_db), current_user: models.User = Depends(require_admin)):
    query = db.query(models.User)
    
    if role:
        query = query.filter(models.User.role == role)
    
    if search:
        search_filter = f"%{search}%"
        query = query.filter(
            models.User.username.ilike(search_filter) |
            models.User.email.ilike(search_filter) |
            models.User.first_name.ilike(search_filter) |
            models.User.last_name.ilike(search_filter)
        )
    
    users = query.order_by(models.User.id).offset(skip).limit(limit).all()
    return users

@router.get("/{user_id}", response_model=schemas.UserResponse)
def get_user(user_id: int, db: Session = Depends(get_db), current_user: models.User = Depends(get_current_user)):
    if current_user.id != user_id and current_user.role != "admin":
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Not enough permissions"
        )
    
    user = db.query(models.User).filter(models.User.id == user_id).first()
    if not user:
        raise HTTPException(
            status_code=404,
            detail="User not found"
        )
    
    return user

@router.put("/{user_id}", response_model=schemas.UserResponse)
def update_user(user_id: int, user_update: schemas.UserUpdate, db: Session = Depends(get_db), current_user: models.User = Depends(get_current_user)):
    if current_user.id != user_id and current_user.role != "admin":
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Not enough permissions"
        )
    
    user = db.query(models.User).filter(models.User.id == user_id).first()
    if not user:
        raise HTTPException(status_code=404, detail="User not found")
    
    if user_update.role is not None and current_user.role != "admin":
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Only admin can change user role"
        )
    
    if user_update.email and user_update.email != user.email:
        existing_user = db.query(models.User).filter(
            models.User.email == user_update.email,
            models.User.id != user_id
        ).first()
        if existing_user:
            raise HTTPException(
                status_code=400,
                detail="Email already registered"
            )
    
    update_data = user_update.dict(exclude_unset=True)
    for field, value in update_data.items():
        setattr(user, field, value)
    
    db.commit()
    db.refresh(user)
    return user

@router.delete("/{user_id}")
def delete_user(user_id: int, db: Session = Depends(get_db), current_user: models.User = Depends(get_current_user)):
    if current_user.id != user_id and current_user.role != "admin":
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Not enough permissions"
        )
    
    if current_user.id == user_id:
        raise HTTPException(
            status_code=400,
            detail="Cannot delete your own account"
        )
    
    user = db.query(models.User).filter(models.User.id == user_id).first()
    if not user:
        raise HTTPException(status_code=404, detail="User not found")
    
    db.delete(user)
    db.commit()
    
    return {"message": "User deleted successfully"}

@router.get("/stats/count")
def get_user_stats(db: Session = Depends(get_db), current_user: models.User = Depends(require_admin)
):
    from sqlalchemy import func
    
    total_users = db.query(func.count(models.User.id)).scalar()
    users_by_role = db.query(
        models.User.role,
        func.count(models.User.id).label('count')
    ).group_by(models.User.role).all()
    
    return {
        "total_users": total_users,
        "users_by_role": dict(users_by_role)
    }