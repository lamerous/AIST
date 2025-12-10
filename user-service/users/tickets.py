from fastapi import APIRouter, Depends, HTTPException, status, Query
from sqlalchemy.orm import Session
from typing import List, Optional

from database import get_db
import models, schemas
from .auth import get_current_user

router = APIRouter()

@router.post("/", response_model=schemas.UserTicketResponse)
def create_user_ticket(ticket: schemas.UserTicketCreate, db: Session = Depends(get_db), current_user: models.User = Depends(get_current_user)):
    existing_ticket = db.query(models.UserTicket).filter(
        models.UserTicket.user_id == current_user.id,
        models.UserTicket.ticket_id == ticket.ticket_id
    ).first()
    
    if existing_ticket:
        raise HTTPException(
            status_code=400,
            detail="Ticket already assigned to user"
        )
    
    db_ticket = models.UserTicket(
        user_id=current_user.id,
        ticket_id=ticket.ticket_id,
        ticket_data=ticket.ticket_data
    )
    
    db.add(db_ticket)
    db.commit()
    db.refresh(db_ticket)
    return db_ticket

@router.get("/", response_model=List[schemas.UserTicketResponse])
def get_user_tickets(skip: int = Query(0, ge=0), limit: int = Query(100, ge=1, le=1000), db: Session = Depends(get_db), current_user: models.User = Depends(get_current_user)):
    tickets = db.query(models.UserTicket).filter(
        models.UserTicket.user_id == current_user.id
    ).order_by(models.UserTicket.purchase_date.desc()).offset(skip).limit(limit).all()
    
    return tickets

@router.get("/{ticket_id}", response_model=schemas.UserTicketResponse)
def get_user_ticket(ticket_id: int, db: Session = Depends(get_db), current_user: models.User = Depends(get_current_user)):
    ticket = db.query(models.UserTicket).filter(
        models.UserTicket.id == ticket_id,
        models.UserTicket.user_id == current_user.id
    ).first()
    
    if not ticket:
        raise HTTPException(
            status_code=404,
            detail="Ticket not found"
        )
    
    return ticket

@router.delete("/{ticket_id}")
def delete_user_ticket(ticket_id: int, db: Session = Depends(get_db), current_user: models.User = Depends(get_current_user)):
    ticket = db.query(models.UserTicket).filter(
        models.UserTicket.id == ticket_id,
        models.UserTicket.user_id == current_user.id
    ).first()
    
    if not ticket:
        raise HTTPException(
            status_code=404,
            detail="Ticket not found"
        )
    
    db.delete(ticket)
    db.commit()
    
    return {"message": "Ticket deleted successfully"}

@router.get("/user/{user_id}", response_model=List[schemas.UserTicketResponse])
def get_tickets_by_user_id(user_id: int, skip: int = Query(0, ge=0), limit: int = Query(100, ge=1, le=1000), db: Session = Depends(get_db), current_user: models.User = Depends(get_current_user)):
    if current_user.id != user_id and current_user.role != "admin":
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Not enough permissions"
        )
    
    tickets = db.query(models.UserTicket).filter(
        models.UserTicket.user_id == user_id
    ).order_by(models.UserTicket.purchase_date.desc()).offset(skip).limit(limit).all()
    
    return tickets