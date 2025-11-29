import asyncpg
from typing import List, Optional
from models import Route, Schedule
from datetime import datetime, date, time

class RouteRepository:
    def __init__(self, connection_pool: asyncpg.Pool):
        self.connection_pool = connection_pool
    
    async def create_table(self):
        """Создание таблицы маршрутов"""
        create_table_query = """
        CREATE TABLE IF NOT EXISTS routes (
            id SERIAL PRIMARY KEY,
            route_number VARCHAR(20) NOT NULL,
            platform_number VARCHAR(10) NOT NULL,
            departure_place VARCHAR(100) NOT NULL,
            destination_place VARCHAR(100) NOT NULL,
            departure_date DATE NOT NULL,
            departure_time TIME NOT NULL,
            destination_time TIME NOT NULL,
            price INTEGER NOT NULL,
            seats INTEGER NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(route_number, departure_date, departure_time)
        );
        
        CREATE INDEX IF NOT EXISTS idx_route_number ON routes(route_number);
        CREATE INDEX IF NOT EXISTS idx_departure_place ON routes(departure_place);
        CREATE INDEX IF NOT EXISTS idx_destination_place ON routes(destination_place);
        CREATE INDEX IF NOT EXISTS idx_departure_date ON routes(departure_date);
        """
        
        async with self.connection_pool.acquire() as connection:
            await connection.execute(create_table_query)
    
    async def add_route(self, route: Route) -> bool:
        """Добавление маршрута"""
        insert_query = """
        INSERT INTO routes (
            route_number, platform_number, departure_place, destination_place,
            departure_date, departure_time, destination_time, price, seats
        ) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)
        ON CONFLICT (route_number, departure_date, departure_time) DO NOTHING
        """
        
        async with self.connection_pool.acquire() as connection:
            result = await connection.execute(
                insert_query,
                route.route_number,
                route.platform_number,
                route.departure_place,
                route.destination_place,
                route.departure_date,
                route.departure_time,
                route.destination_time,
                route.price,
                route.seats
            )
            return "INSERT" in result
    
    async def get_route_by_id(self, route_id: int) -> Optional[Route]:
        """Получение маршрута по ID"""
        query = "SELECT * FROM routes WHERE id = $1"
        
        async with self.connection_pool.acquire() as connection:
            row = await connection.fetchrow(query, route_id)
            if row:
                return self._row_to_route(row)
            return None
    
    async def get_all_routes(self) -> List[Route]:
        """Получение всех маршрутов"""
        query = "SELECT * FROM routes ORDER BY departure_date, departure_time"
        
        async with self.connection_pool.acquire() as connection:
            rows = await connection.fetch(query)
            return [self._row_to_route(row) for row in rows]
    
    async def find_routes_by_number(self, route_number: str) -> List[Route]:
        """Поиск маршрутов по номеру"""
        query = "SELECT * FROM routes WHERE route_number = $1 ORDER BY departure_date, departure_time"
        
        async with self.connection_pool.acquire() as connection:
            rows = await connection.fetch(query, route_number)
            return [self._row_to_route(row) for row in rows]
    
    async def find_routes_by_departure(self, departure_place: str) -> List[Route]:
        """Поиск маршрутов по месту отправления"""
        query = "SELECT * FROM routes WHERE LOWER(departure_place) LIKE LOWER($1) ORDER BY departure_date, departure_time"
        
        async with self.connection_pool.acquire() as connection:
            rows = await connection.fetch(query, f"%{departure_place}%")
            return [self._row_to_route(row) for row in rows]
    
    async def find_routes_by_destination(self, destination_place: str) -> List[Route]:
        """Поиск маршрутов по месту назначения"""
        query = "SELECT * FROM routes WHERE LOWER(destination_place) LIKE LOWER($1) ORDER BY departure_date, departure_time"
        
        async with self.connection_pool.acquire() as connection:
            rows = await connection.fetch(query, f"%{destination_place}%")
            return [self._row_to_route(row) for row in rows]
    
    async def find_routes_by_direction(self, departure_place: str, destination_place: str) -> List[Route]:
        """Поиск маршрутов по направлению"""
        query = """
        SELECT * FROM routes 
        WHERE LOWER(departure_place) LIKE LOWER($1) 
        AND LOWER(destination_place) LIKE LOWER($2)
        ORDER BY departure_date, departure_time
        """
        
        async with self.connection_pool.acquire() as connection:
            rows = await connection.fetch(query, f"%{departure_place}%", f"%{destination_place}%")
            return [self._row_to_route(row) for row in rows]
    
    async def update_route(self, route_id: int, route: Route) -> bool:
        """Обновление маршрута"""
        update_query = """
        UPDATE routes SET
            route_number = $1,
            platform_number = $2,
            departure_place = $3,
            destination_place = $4,
            departure_date = $5,
            departure_time = $6,
            destination_time = $7,
            price = $8,
            seats = $9
        WHERE id = $10
        """
        
        async with self.connection_pool.acquire() as connection:
            result = await connection.execute(
                update_query,
                route.route_number,
                route.platform_number,
                route.departure_place,
                route.destination_place,
                route.departure_date,
                route.departure_time,
                route.destination_time,
                route.price,
                route.seats,
                route_id
            )
            return "UPDATE" in result
    
    async def delete_route(self, route_id: int) -> bool:
        """Удаление маршрута по ID"""
        query = "DELETE FROM routes WHERE id = $1"
        
        async with self.connection_pool.acquire() as connection:
            result = await connection.execute(query, route_id)
            return "DELETE" in result
    
    async def delete_route_by_number(self, route_number: str) -> bool:
        """Удаление маршрутов по номеру"""
        query = "DELETE FROM routes WHERE route_number = $1"
        
        async with self.connection_pool.acquire() as connection:
            result = await connection.execute(query, route_number)
            return "DELETE" in result
    
    def _row_to_route(self, row) -> Route:
        """Преобразование строки БД в объект Route"""
        return Route(
            route_number=row['route_number'],
            platform_number=row['platform_number'],
            departure_place=row['departure_place'],
            destination_place=row['destination_place'],
            departure_date=row['departure_date'],
            departure_time=row['departure_time'],
            destination_time=row['destination_time'],
            price=row['price'],
            seats=row['seats']
        )