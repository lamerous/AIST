# tickets.py
from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session, joinedload
from datetime import date, datetime
import uuid
import models, schemas
from database import get_db
from dependencies import get_current_user

router = APIRouter()

def generate_ticket_number():
    timestamp = datetime.now().strftime("%Y%m%d%H%M%S")
    random_part = uuid.uuid4().hex[:6].upper()
    return f"TKT-{timestamp}-{random_part}"

@router.post("/", response_model=schemas.TicketResponse)
def create_ticket(
    ticket: schemas.TicketCreate,
    db: Session = Depends(get_db),
    current_user: dict = Depends(get_current_user)
):
    route = db.query(models.Route).filter(models.Route.id == ticket.route_id).first()
    if not route:
        raise HTTPException(status_code=404, detail="Route not found")
    
    if route.available_seats <= 0:
        raise HTTPException(status_code=400, detail="No available seats")
    
    ticket_number = generate_ticket_number()
    
    purchase_date = date.today()
    
    purchase_price = route.price
    
    db_ticket = models.Ticket(
        ticket_number=ticket_number,
        purchase_date=purchase_date,
        purchase_price=purchase_price,
        route_id=ticket.route_id,
        user_id=current_user["user_id"]
    )
    
    route.available_seats -= 1
    
    db.add(db_ticket)
    db.commit()
    db.refresh(db_ticket)
    
    return create_ticket_response(db_ticket, db, current_user)

@router.get("/", response_model=list[schemas.TicketResponse])
def read_tickets(
    skip: int = 0,
    limit: int = 100,
    db: Session = Depends(get_db),
    current_user: dict = Depends(get_current_user)
):
    tickets = (
        db.query(models.Ticket)
        .options(joinedload(models.Ticket.route))
        .filter(models.Ticket.user_id == current_user["user_id"])
        .offset(skip)
        .limit(limit)
        .all()
    )
    
    result = []
    for ticket in tickets:
        result.append(create_ticket_response(ticket, db, current_user))
    
    return result

@router.get("/{ticket_id}", response_model=schemas.TicketResponse)
def read_ticket(
    ticket_id: int,
    db: Session = Depends(get_db),
    current_user: dict = Depends(get_current_user)
):
    ticket = (
        db.query(models.Ticket)
        .options(joinedload(models.Ticket.route))
        .filter(
            models.Ticket.id == ticket_id,
            models.Ticket.user_id == current_user["user_id"]
        )
        .first()
    )
    
    if not ticket:
        raise HTTPException(status_code=404, detail="Ticket not found")
    
    return create_ticket_response(ticket, db, current_user)

def create_ticket_response(ticket: models.Ticket, db: Session, current_user: dict) -> schemas.TicketResponse:
    response = schemas.TicketResponse(
        id=ticket.id,
        ticket_number=ticket.ticket_number,
        purchase_date=ticket.purchase_date,
        purchase_price=ticket.purchase_price,
        route_id=ticket.route_id,
        user_id=current_user["user_id"]
    )
    
    if ticket.route:
        response.departure_time = ticket.route.departure_time
        response.arrival_time = ticket.route.arrival_time
        
        path = db.query(models.Path).filter(
            models.Path.route_number == ticket.route.path_number
        ).first()
        
        if path and path.stops_array:
            stops_array = path.stops_array
            
            if isinstance(stops_array, dict) and "stops" in stops_array:
                stops = stops_array["stops"]
                
                if stops and isinstance(stops, list) and len(stops) > 0:
                    first_stop = stops[0]
                    last_stop = stops[-1]
                    
                    if isinstance(first_stop, dict):
                        response.start_stop = first_stop.get("name", "")
                        response.end_stop = last_stop.get("name", "")
                    
                    elif isinstance(first_stop, int):
                        start_stop = db.query(models.Stop).filter(
                            models.Stop.id == first_stop
                        ).first()
                        if start_stop:
                            response.start_stop = start_stop.stop_name
                        
                        end_stop = db.query(models.Stop).filter(
                            models.Stop.id == last_stop
                        ).first()
                        if end_stop:
                            response.end_stop = end_stop.stop_name
    
    return response