import asyncpg
from typing import List, Optional
from models import BusStop, BusStopList

class BusStopRepository:
    def __init__(self, connection_pool: asyncpg.Pool):
        self.connection_pool = connection_pool
    
    async def create_table(self):
        create_table_query = """
        CREATE TABLE IF NOT EXISTS bus_stops (
            id SERIAL PRIMARY KEY,
            stop_id INTEGER NOT NULL,
            stop_name VARCHAR(100) NOT NULL,
            stop_address VARCHAR(200) NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(stop_id, stop_name, stop_address)
        );
        
        CREATE INDEX IF NOT EXISTS idx_stop_id ON bus_stops(stop_id);
        CREATE INDEX IF NOT EXISTS idx_stop_name ON bus_stops(stop_name);
        CREATE INDEX IF NOT EXISTS idx_stop_address ON bus_stops(stop_address);
        """
        
        async with self.connection_pool.acquire() as connection:
            await connection.execute(create_table_query)
    
    async def add_busstop(self, busstop: BusStop) -> bool:
        insert_query = """
        INSERT INTO bus_stops (stop_id, stop_name, stop_address) 
        VALUES ($1, $2, $3)
        ON CONFLICT (stop_id, stop_name, stop_address) DO NOTHING
        """
        
        async with self.connection_pool.acquire() as connection:
            result = await connection.execute(
                insert_query,
                busstop.stop_id,
                busstop.stop_name,
                busstop.stop_address
            )
            return "INSERT" in result
    
    async def get_busstop_by_id(self, busstop_id: int) -> Optional[BusStop]:
        query = "SELECT * FROM bus_stops WHERE stop_id = $1"
        
        async with self.connection_pool.acquire() as connection:
            row = await connection.fetchrow(query, busstop_id)
            if row:
                return self._row_to_busstop(row)
            return None
    
    async def get_all_busstops(self) -> List[BusStop]:
        query = "SELECT * FROM bus_stops"
        
        async with self.connection_pool.acquire() as connection:
            rows = await connection.fetch(query)
            return [self._row_to_busstop(row) for row in rows]
    
    async def find_busstops_by_name(self, stop_name: str) -> List[BusStop]:
        query = "SELECT * FROM bus_stops WHERE LOWER(stop_name) LIKE LOWER($1)"
        
        async with self.connection_pool.acquire() as connection:
            rows = await connection.fetch(query, f"%{stop_name}%")
            return [self._row_to_busstop(row) for row in rows]
    
    async def find_busstops_by_address(self, stop_address: str) -> List[BusStop]:
        query = "SELECT * FROM bus_stops WHERE LOWER(stop_address) LIKE LOWER($1)"
        
        async with self.connection_pool.acquire() as connection:
            rows = await connection.fetch(query, f"%{stop_address}%")
            return [self._row_to_busstop(row) for row in rows]
    
    async def update_busstop(self, busstop_id: int, busstop: BusStop) -> bool:
        update_query = """
        UPDATE bus_stops SET
            stop_id = $1,
            stop_name = $2,
            stop_address = $3
        WHERE stop_id = $4
        """
        
        async with self.connection_pool.acquire() as connection:
            result = await connection.execute(
                update_query,
                busstop.stop_name,
                busstop.stop_address,
                busstop_id
            )
            return "UPDATE" in result
    
    async def delete_busstop(self, busstop_id: int) -> bool:
        query = "DELETE FROM bus_stops WHERE stop_id = $1"
        
        async with self.connection_pool.acquire() as connection:
            result = await connection.execute(query, busstop_id)
            return "DELETE" in result
    
    async def delete_busstop_by_name(self, stop_name: str) -> bool:
        query = "DELETE FROM bus_stops WHERE stop_name = $1"
        
        async with self.connection_pool.acquire() as connection:
            result = await connection.execute(query, stop_name)
            return "DELETE" in result
    
    def _row_to_busstop(self, row) -> BusStop:
        return BusStop(
            stop_id=row['stop_id'],
            stop_name=row['stop_name'],
            stop_address=row['stop_address']
        )