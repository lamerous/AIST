from typing import List, Optional
from models import Route, Schedule
from repository import RouteRepository
from datetime import datetime, date, time

class RouteService:
    def __init__(self, repository: RouteRepository):
        self.repository = repository
    
    async def add_route(self, route_data: dict) -> dict:
        """Добавление нового маршрута"""
        try:
            route = Route.from_dict(route_data)
            success = await self.repository.add_route(route)
            
            if success:
                return {"success": True, "message": "Маршрут успешно добавлен"}
            else:
                return {"success": False, "message": "Маршрут уже существует"}
                
        except Exception as e:
            return {"success": False, "message": f"Ошибка при добавлении маршрута: {str(e)}"}
    
    async def get_all_routes(self) -> dict:
        """Получение всех маршрутов"""
        try:
            routes = await self.repository.get_all_routes()
            return {
                "success": True,
                "routes": [route.to_dict() for route in routes]
            }
        except Exception as e:
            return {"success": False, "message": f"Ошибка при получении маршрутов: {str(e)}"}
    
    async def find_routes_by_number(self, route_number: str) -> dict:
        """Поиск маршрутов по номеру"""
        try:
            routes = await self.repository.find_routes_by_number(route_number)
            return {
                "success": True,
                "routes": [route.to_dict() for route in routes]
            }
        except Exception as e:
            return {"success": False, "message": f"Ошибка при поиске маршрутов: {str(e)}"}
    
    async def find_routes_by_departure(self, departure_place: str) -> dict:
        """Поиск маршрутов по месту отправления"""
        try:
            routes = await self.repository.find_routes_by_departure(departure_place)
            return {
                "success": True,
                "routes": [route.to_dict() for route in routes]
            }
        except Exception as e:
            return {"success": False, "message": f"Ошибка при поиске маршрутов: {str(e)}"}
    
    async def find_routes_by_destination(self, destination_place: str) -> dict:
        """Поиск маршрутов по месту назначения"""
        try:
            routes = await self.repository.find_routes_by_destination(destination_place)
            return {
                "success": True,
                "routes": [route.to_dict() for route in routes]
            }
        except Exception as e:
            return {"success": False, "message": f"Ошибка при поиске маршрутов: {str(e)}"}
    
    async def find_routes_by_direction(self, departure_place: str, destination_place: str) -> dict:
        """Поиск маршрутов по направлению"""
        try:
            routes = await self.repository.find_routes_by_direction(departure_place, destination_place)
            return {
                "success": True,
                "routes": [route.to_dict() for route in routes]
            }
        except Exception as e:
            return {"success": False, "message": f"Ошибка при поиске маршрутов: {str(e)}"}
    
    async def update_route(self, route_id: int, route_data: dict) -> dict:
        """Обновление маршрута"""
        try:
            route = Route.from_dict(route_data)
            success = await self.repository.update_route(route_id, route)
            
            if success:
                return {"success": True, "message": "Маршрут успешно обновлен"}
            else:
                return {"success": False, "message": "Маршрут не найден"}
                
        except Exception as e:
            return {"success": False, "message": f"Ошибка при обновлении маршрута: {str(e)}"}
    
    async def delete_route(self, route_id: int) -> dict:
        """Удаление маршрута по ID"""
        try:
            success = await self.repository.delete_route(route_id)
            
            if success:
                return {"success": True, "message": "Маршрут успешно удален"}
            else:
                return {"success": False, "message": "Маршрут не найден"}
                
        except Exception as e:
            return {"success": False, "message": f"Ошибка при удалении маршрута: {str(e)}"}