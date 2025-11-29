from typing import List, Optional
from models import BusStop, BusStopList
from repository import BusStopRepository

class BusStopService:
    def __init__(self, repository: BusStopRepository):
        self.repository = repository
    
    async def add_busstop(self, busstop_data: dict) -> dict:
        try:
            busstop = BusStop.from_dict(busstop_data)
            success = await self.repository.add_busstop(busstop)
            
            if success:
                return {"success": True, "message": "Остановка успешно добавлена"}
            else:
                return {"success": False, "message": "Остановка уже существует"}
                
        except Exception as e:
            return {"success": False, "message": f"Ошибка при добавлении остановки: {str(e)}"}
    
    async def get_all_busstops(self) -> dict:
        try:
            busstops = await self.repository.get_all_busstops()
            return {
                "success": True,
                "busstops": [busstop.to_dict() for busstop in busstops]
            }
        except Exception as e:
            return {"success": False, "message": f"Ошибка при получении остановок: {str(e)}"}
    
    async def get_busstop_by_id(self, busstop_id: int) -> dict:
        try:
            busstop = await self.repository.get_busstop_by_id(busstop_id)
            if busstop:
                return {
                    "success": True,
                    "busstop": busstop.to_dict()
                }
            else:
                return {"success": False, "message": "Остановка не найдена"}
        except Exception as e:
            return {"success": False, "message": f"Ошибка при получении остановки: {str(e)}"}
    
    async def find_busstops_by_name(self, stop_name: str) -> dict:
        try:
            busstops = await self.repository.find_busstops_by_name(stop_name)
            return {
                "success": True,
                "busstops": [busstop.to_dict() for busstop in busstops]
            }
        except Exception as e:
            return {"success": False, "message": f"Ошибка при поиске остановок: {str(e)}"}
    
    async def find_busstops_by_address(self, stop_address: str) -> dict:
        try:
            busstops = await self.repository.find_busstops_by_address(stop_address)
            return {
                "success": True,
                "busstops": [busstop.to_dict() for busstop in busstops]
            }
        except Exception as e:
            return {"success": False, "message": f"Ошибка при поиске остановок: {str(e)}"}
    
    async def update_busstop(self, busstop_id: int, busstop_data: dict) -> dict:
        try:
            busstop = BusStop.from_dict(busstop_data)
            success = await self.repository.update_busstop(busstop_id, busstop)
            
            if success:
                return {"success": True, "message": "Остановка успешно обновлена"}
            else:
                return {"success": False, "message": "Остановка не найдена"}
                
        except Exception as e:
            return {"success": False, "message": f"Ошибка при обновлении остановки: {str(e)}"}
    
    async def delete_busstop(self, busstop_id: int) -> dict:
        try:
            success = await self.repository.delete_busstop(busstop_id)
            
            if success:
                return {"success": True, "message": "Остановка успешно удалена"}
            else:
                return {"success": False, "message": "Остановка не найдена"}
                
        except Exception as e:
            return {"success": False, "message": f"Ошибка при удалении остановки: {str(e)}"}