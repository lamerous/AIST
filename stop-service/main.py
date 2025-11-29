from fastapi import FastAPI, HTTPException, Depends
from fastapi.middleware.cors import CORSMiddleware
import asyncpg
from typing import List, Optional
import os

from models import BusStop
from repository import BusStopRepository
from service import BusStopService

app = FastAPI(title="Bus Stop Microservice", version="1.0.0")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

async def get_database():
    database_url = os.getenv("DATABASE_URL")
    pool = await asyncpg.create_pool(database_url)
    try:
        yield pool
    finally:
        await pool.close()

async def get_repository(db_pool=Depends(get_database)):
    return BusStopRepository(db_pool)

async def get_service(repository: BusStopRepository = Depends(get_repository)):
    return BusStopService(repository)

@app.on_event("startup")
async def startup_event():
    db_pool = await asyncpg.create_pool(
        os.getenv("DATABASE_URL")
    )
    repository = BusStopRepository(db_pool)
    await repository.create_table()
    await db_pool.close()

@app.get("/")
async def root():
    return {"message": "Bus Stop Microservice", "version": "1.0.0"}

@app.post("/busstops", response_model=dict)
async def create_busstop(busstop_data: dict, service: BusStopService = Depends(get_service)):
    return await service.add_busstop(busstop_data)

@app.get("/busstops", response_model=dict)
async def get_busstops(service: BusStopService = Depends(get_service)):
    return await service.get_all_busstops()

@app.get("/busstops/name/{stop_name}", response_model=dict)
async def get_busstops_by_name(stop_name: str, service: BusStopService = Depends(get_service)):
    return await service.find_busstops_by_name(stop_name)

@app.get("/busstops/address/{stop_address}", response_model=dict)
async def get_busstops_by_address(stop_address: str, service: BusStopService = Depends(get_service)):
    return await service.find_busstops_by_address(stop_address)

@app.get("/busstops/{busstop_id}", response_model=dict)
async def get_busstop_by_id(busstop_id: int, service: BusStopService = Depends(get_service)):
    return await service.get_busstop_by_id(busstop_id)

@app.put("/busstops/{busstop_id}", response_model=dict)
async def update_busstop(busstop_id: int, busstop_data: dict, service: BusStopService = Depends(get_service)):
    return await service.update_busstop(busstop_id, busstop_data)

@app.delete("/busstops/{busstop_id}", response_model=dict)
async def delete_busstop(busstop_id: int, service: BusStopService = Depends(get_service)):
    return await service.delete_busstop(busstop_id)

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)