from pydantic import BaseModel, ConfigDict
from datetime import date, time
from typing import Optional, Any, List

class StopBase(BaseModel):
    stop_name: str
    stop_address: str

class StopIds(BaseModel):
    stop_ids: List[int]

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
    path_number: str
    platform_number: str
    price: int
    available_seats: int
    departure_date: date
    departure_time: time
    arrival_time: time

class RouteCreate(RouteBase):
    pass

class Route(RouteBase):
    id: int
    path: Optional[dict] = None
    model_config = ConfigDict(from_attributes=True)

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