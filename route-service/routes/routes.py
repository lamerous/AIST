from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
import models, schemas
from database import get_db

router = APIRouter()

@router.post("/", response_model=schemas.Route)
def create_route(route: schemas.RouteCreate, db: Session = Depends(get_db)):
    db_route = models.Route(**route.dict())
    db.add(db_route)
    db.commit()
    db.refresh(db_route)
    return db_route

@router.get("/", response_model=list[schemas.Route])
def read_routes(skip: int = 0, limit: int = 100, db: Session = Depends(get_db)):
    routes = db.query(models.Route).offset(skip).limit(limit).all()
    return routes

@router.get("/{route_id}", response_model=schemas.Route)
def read_route(route_id: int, db: Session = Depends(get_db)):
    route = db.query(models.Route).filter(models.Route.id == route_id).first()
    if route is None:
        raise HTTPException(status_code=404, detail="Route not found")
    return route

@router.put("/{route_id}", response_model=schemas.Route)
def update_route(route_id: int, route: schemas.RouteCreate, db: Session = Depends(get_db)):
    db_route = db.query(models.Route).filter(models.Route.id == route_id).first()
    if db_route is None:
        raise HTTPException(status_code=404, detail="Route not found")
    for key, value in route.dict().items():
        setattr(db_route, key, value)
    db.commit()
    db.refresh(db_route)
    return db_route

@router.delete("/{route_id}")
def delete_route(route_id: int, db: Session = Depends(get_db)):
    db_route = db.query(models.Route).filter(models.Route.id == route_id).first()
    if db_route is None:
        raise HTTPException(status_code=404, detail="Route not found")
    db.delete(db_route)
    db.commit()
    return {"message": "Route deleted"}