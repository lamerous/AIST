from sqlalchemy import Column, Integer, String, Date, DateTime, JSON, ForeignKey
from sqlalchemy.orm import relationship
from sqlalchemy.sql import func
from datetime import datetime
from database import Base

class User(Base):
    __tablename__ = "users"
    
    id = Column(Integer, primary_key=True, index=True)
    username = Column(String(50), unique=True, nullable=False, index=True)
    first_name = Column(String(50))
    last_name = Column(String(50))
    middle_name = Column(String(50))
    role = Column(String(20), default="passenger")
    password_hash = Column(String(255), nullable=False)
    registration_date = Column(Date, default=datetime.utcnow().date())
    email = Column(String(100), unique=True, index=True)
    phone_number = Column(String(20))
    birth_date = Column(Date)
    created_at = Column(DateTime(timezone=True), server_default=func.now())
    updated_at = Column(DateTime(timezone=True), onupdate=func.now())
    
    tickets = relationship("UserTicket", back_populates="user", cascade="all, delete-orphan")
    
    def has_role(self, required_roles):
        if isinstance(required_roles, str):
            required_roles = [required_roles]
        return self.role in required_roles

class UserTicket(Base):
    __tablename__ = "users_tickets"
    
    id = Column(Integer, primary_key=True, index=True)
    user_id = Column(Integer, ForeignKey("users.id", ondelete="CASCADE"), nullable=False)
    ticket_id = Column(Integer, nullable=False, index=True)
    ticket_data = Column(JSON)
    purchase_date = Column(DateTime(timezone=True), server_default=func.now())
    
    user = relationship("User", back_populates="tickets")