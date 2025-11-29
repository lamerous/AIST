from dataclasses import dataclass
from typing import List, Optional

@dataclass
class BusStop:
    stop_id: int
    stop_name: str
    stop_address: str
    
    def to_dict(self) -> dict:
        return {
            'stop_id': self.stop_id,
            'stop_name': self.stop_name,
            'stop_address': self.stop_address
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'BusStop':
        return cls(
            stop_id=data['stop_id'],
            stop_name=data['stop_name'],
            stop_address=data['stop_address']
        )

@dataclass
class BusStopList:
    busstops: List[BusStop] = None
    
    def __post_init__(self):
        if self.busstops is None:
            self.busstops = []
    
    def add_busstop(self, busstop: BusStop):
        self.busstops.append(busstop)
    
    def remove_busstop(self, stop_name: str) -> bool:
        initial_count = len(self.busstops)
        self.busstops = [b for b in self.busstops if b.stop_name != stop_name]
        return len(self.busstops) < initial_count
    
    def find_busstops_by_name(self, stop_name: str) -> List[BusStop]:
        return [b for b in self.busstops if stop_name.lower() in b.stop_name.lower()]
    
    def find_busstops_by_address(self, stop_address: str) -> List[BusStop]:
        return [b for b in self.busstops if stop_address.lower() in b.stop_address.lower()]
    
    def to_dict(self) -> dict:
        return {
            'busstops': [busstop.to_dict() for busstop in self.busstops]
        }