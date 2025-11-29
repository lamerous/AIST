from dataclasses import dataclass
from datetime import datetime, date, time
from typing import List, Optional

@dataclass
class Route:
    route_number: str
    platform_number: str
    departure_place: str
    destination_place: str
    departure_date: date
    departure_time: time
    destination_time: time
    price: int
    seats: int
    
    def to_dict(self) -> dict:
        return {
            'route_number': self.route_number,
            'platform_number': self.platform_number,
            'departure_place': self.departure_place,
            'destination_place': self.destination_place,
            'departure_date': self.departure_date.isoformat(),
            'departure_time': self.departure_time.strftime('%H:%M'),
            'destination_time': self.destination_time.strftime('%H:%M'),
            'price': self.price,
            'seats': self.seats
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'Route':
        return cls(
            route_number=data['route_number'],
            platform_number=data['platform_number'],
            departure_place=data['departure_place'],
            destination_place=data['destination_place'],
            departure_date=datetime.strptime(data['departure_date'], '%Y-%m-%d').date(),
            departure_time=datetime.strptime(data['departure_time'], '%H:%M').time(),
            destination_time=datetime.strptime(data['destination_time'], '%H:%M').time(),
            price=data['price'],
            seats=data['seats']
        )

@dataclass
class Schedule:
    routes: List[Route] = None
    
    def __post_init__(self):
        if self.routes is None:
            self.routes = []
    
    def add_route(self, route: Route):
        self.routes.append(route)
    
    def remove_route(self, route_number: str) -> bool:
        initial_count = len(self.routes)
        self.routes = [r for r in self.routes if r.route_number != route_number]
        return len(self.routes) < initial_count
    
    def find_routes_by_number(self, route_number: str) -> List[Route]:
        return [r for r in self.routes if r.route_number == route_number]
    
    def find_routes_by_departure(self, departure_place: str) -> List[Route]:
        return [r for r in self.routes if departure_place.lower() in r.departure_place.lower()]
    
    def find_routes_by_destination(self, destination_place: str) -> List[Route]:
        return [r for r in self.routes if destination_place.lower() in r.destination_place.lower()]
    
    def find_routes_by_direction(self, departure_place: str, destination_place: str) -> List[Route]:
        return [r for r in self.routes 
                if departure_place.lower() in r.departure_place.lower() 
                and destination_place.lower() in r.destination_place.lower()]
    
    def to_dict(self) -> dict:
        return {
            'routes': [route.to_dict() for route in self.routes]
        }