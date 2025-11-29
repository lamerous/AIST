from fastapi import FastAPI, HTTPException, Depends
from fastapi.middleware.cors import CORSMiddleware
import asyncpg
from typing import List, Optional
import os

from models import Route
from repository import RouteRepository
from service import RouteService

app = FastAPI(title="Route Schedule Microservice", version="1.0.0")

# CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Dependency for database connection
async def get_database():
    database_url = os.getenv("DATABASE_URL")
    pool = await asyncpg.create_pool(database_url)
    try:
        yield pool
    finally:
        await pool.close()

async def get_repository(db_pool=Depends(get_database)):
    return RouteRepository(db_pool)

async def get_service(repository: RouteRepository = Depends(get_repository)):
    return RouteService(repository)

@app.on_event("startup")
async def startup_event():
    db_pool = await asyncpg.create_pool(
        os.getenv("DATABASE_URL")
    )
    repository = RouteRepository(db_pool)
    await repository.create_table()
    await db_pool.close()

@app.get("/")
async def root():
    return {"message": "Route Schedule Microservice", "version": "1.0.0"}

@app.post("/routes", response_model=dict)
async def create_route(route_data: dict, service: RouteService = Depends(get_service)):
    return await service.add_route(route_data)

@app.get("/routes", response_model=dict)
async def get_routes(service: RouteService = Depends(get_service)):
    return await service.get_all_routes()

@app.get("/routes/number/{route_number}", response_model=dict)
async def get_routes_by_number(route_number: str, service: RouteService = Depends(get_service)):
    return await service.find_routes_by_number(route_number)

@app.get("/routes/departure/{departure_place}", response_model=dict)
async def get_routes_by_departure(departure_place: str, service: RouteService = Depends(get_service)):
    return await service.find_routes_by_departure(departure_place)

@app.get("/routes/destination/{destination_place}", response_model=dict)
async def get_routes_by_destination(destination_place: str, service: RouteService = Depends(get_service)):
    return await service.find_routes_by_destination(destination_place)

@app.get("/routes/direction/{departure_place}/{destination_place}", response_model=dict)
async def get_routes_by_direction(departure_place: str, destination_place: str, service: RouteService = Depends(get_service)):
    return await service.find_routes_by_direction(departure_place, destination_place)

@app.put("/routes/{route_id}", response_model=dict)
async def update_route(route_id: int, route_data: dict, service: RouteService = Depends(get_service)):
    return await service.update_route(route_id, route_data)

@app.delete("/routes/{route_id}", response_model=dict)
async def delete_route(route_id: int, service: RouteService = Depends(get_service)):
    return await service.delete_route(route_id)

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)