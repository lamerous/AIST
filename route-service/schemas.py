from pydantic import BaseModel
from datetime import date, time
from typing import Optional, Any

class StopBase(BaseModel):
    stop_name: str
    stop_address: str

class StopCreate(StopBase):
    pass

class Stop(StopBase):
    id: int
    class Config:
        orm_mode = True

class PathBase(BaseModel):
    route_number: str
    stops_array: dict

class PathCreate(PathBase):
    pass

class Path(PathBase):
    id: int
    class Config:
        orm_mode = True

class RouteBase(BaseModel):
    route_number: str
    platform_number: str
    price: int
    available_seats: int
    departure_date: date
    departure_time: time
    arrival_time: time
    path_id: int

class RouteCreate(RouteBase):
    pass

class Route(RouteBase):
    id: int
    class Config:
        orm_mode = True

class TicketBase(BaseModel):
    ticket_number: str
    purchase_date: date
    purchase_price: float
    route_id: int

class TicketCreate(TicketBase):
    pass

class Ticket(TicketBase):
    id: int
    class Config:
        orm_mode = True