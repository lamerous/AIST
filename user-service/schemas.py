from pydantic import BaseModel, EmailStr, validator, Field
from typing import Optional, Dict, Any
from datetime import date, datetime
from enum import Enum

class UserRole(str, Enum):
    PASSENGER = "passenger"
    DISPATCHER = "dispatcher"
    CASHIER = "cashier"
    ADMIN = "admin"

class UserBase(BaseModel):
    username: str = Field(..., min_length=3, max_length=50)
    email: EmailStr
    first_name: str = Field(..., max_length=50)
    last_name: str = Field(..., max_length=50)
    middle_name: Optional[str] = Field(None, max_length=50)
    phone_number: Optional[str] = Field(None, max_length=20)
    birth_date: Optional[date] = None

class UserCreate(UserBase):
    password: str = Field(..., min_length=8)
    
    @validator('password')
    def password_strength(cls, v):
        if len(v) < 8:
            raise ValueError('Password must be at least 8 characters long')
        return v

class UserUpdate(BaseModel):
    email: Optional[EmailStr] = None
    first_name: Optional[str] = Field(None, max_length=50)
    last_name: Optional[str] = Field(None, max_length=50)
    middle_name: Optional[str] = Field(None, max_length=50)
    phone_number: Optional[str] = Field(None, max_length=20)
    birth_date: Optional[date] = None
    role: Optional[UserRole] = None

class UserResponse(UserBase):
    id: int
    registration_date: date
    created_at: Optional[datetime] = None
    updated_at: Optional[datetime] = None
    
    class Config:
        from_attributes = True

class Token(BaseModel):
    access_token: str
    token_type: str
    user: UserResponse

class TokenData(BaseModel):
    username: Optional[str] = None
    user_id: Optional[int] = None
    role: Optional[str] = None

class LoginRequest(BaseModel):
    username: str
    password: str

class UserTicketBase(BaseModel):
    ticket_id: int
    ticket_data: Optional[Dict[str, Any]] = None

class UserTicketCreate(UserTicketBase):
    user_id: Optional[int] = None

class UserTicketResponse(UserTicketBase):
    id: int
    user_id: int
    purchase_date: datetime
    
    class Config:
        from_attributes = True

class ChangePassword(BaseModel):
    current_password: str
    new_password: str = Field(..., min_length=6)
    
    @validator('new_password')
    def passwords_not_match(cls, v, values):
        if 'current_password' in values and v == values['current_password']:
            raise ValueError('New password must be different from current password')
        return v