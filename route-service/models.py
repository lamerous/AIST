from sqlalchemy import Column, Integer, String, Date, Time, JSON, ForeignKey, Numeric
from sqlalchemy.orm import relationship
from database import Base

class Stop(Base):
    __tablename__ = "stops"
    id = Column(Integer, primary_key=True, index=True)
    stop_name = Column(String(100))
    stop_address = Column(String(200))

class Path(Base):
    __tablename__ = "paths"
    id = Column(Integer, primary_key=True, index=True)
    route_number = Column(String(20))
    stops_array = Column(JSON)

class Route(Base):
    __tablename__ = "routes"
    id = Column(Integer, primary_key=True, index=True)
    path_number = Column(String(20))
    platform_number = Column(String(10))
    price = Column(Integer)
    available_seats = Column(Integer)
    departure_date = Column(Date)
    departure_time = Column(Time)
    arrival_time = Column(Time)

class Ticket(Base):
    __tablename__ = "tickets"
    id = Column(Integer, primary_key=True, index=True)
    ticket_number = Column(String(50))
    purchase_date = Column(Date)
    purchase_price = Column(Numeric(10, 2))
    route_id = Column(Integer, ForeignKey("routes.id", ondelete="CASCADE"))
    user_id = Column(Integer)
    
    route = relationship("Route")