from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
import models, schemas
from database import get_db
from dependencies import get_current_user, require_dispatcher_role

router = APIRouter()

@router.post("/", response_model=schemas.Route)
def create_route(
    route: schemas.RouteCreate,
    db: Session = Depends(get_db),
    current_user: dict = Depends(require_dispatcher_role)
):
    db_route = models.Route(**route.dict())
    db.add(db_route)
    db.commit()
    db.refresh(db_route)
    return db_route

@router.get("/", response_model=list[schemas.Route])
def read_routes(
    skip: int = 0, 
    limit: int = 100, 
    db: Session = Depends(get_db)
):
    routes_with_paths = (
        db.query(models.Route, models.Path)
        .join(
            models.Path, 
            models.Route.path_number == models.Path.route_number,
            isouter=True
        )
        .offset(skip)
        .limit(limit)
        .all()
    )
    
    result = []
    for route, path in routes_with_paths:
        route_data = schemas.Route.from_orm(route)
        
        if path:
            route_data.path = {
                "id": path.id,
                "route_number": path.route_number,
                "stops_array": path.stops_array
            }
        else:
            route_data.path = None
        
        result.append(route_data)
    
    return result

@router.get("/{route_id}", response_model=schemas.Route)
def read_route(
    route_id: int, 
    db: Session = Depends(get_db)
):
    route_with_path = (
        db.query(models.Route, models.Path)
        .join(
            models.Path,
            models.Route.path_number == models.Path.route_number,
            isouter=True
        )
        .filter(models.Route.id == route_id)
        .first()
    )
    
    if not route_with_path or not route_with_path[0]:
        raise HTTPException(status_code=404, detail="Route not found")
    
    route, path = route_with_path
    
    route_data = schemas.Route.from_orm(route)
    
    if path:
        route_data.path = {
            "id": path.id,
            "route_number": path.route_number,
            "stops_array": path.stops_array
        }
    else:
        route_data.path = None
    
    return route_data

@router.put("/{route_id}", response_model=schemas.Route)
def update_route(
    route_id: int, 
    route: schemas.RouteCreate, 
    db: Session = Depends(get_db),
    current_user: dict = Depends(require_dispatcher_role)
):
    db_route = db.query(models.Route).filter(models.Route.id == route_id).first()
    if db_route is None:
        raise HTTPException(status_code=404, detail="Route not found")
    for key, value in route.dict().items():
        setattr(db_route, key, value)
    db.commit()
    db.refresh(db_route)
    return db_route

@router.delete("/{route_id}")
def delete_route(
    route_id: int, 
    db: Session = Depends(get_db),
    current_user: dict = Depends(require_dispatcher_role)
):
    db_route = db.query(models.Route).filter(models.Route.id == route_id).first()
    if db_route is None:
        raise HTTPException(status_code=404, detail="Route not found")
    db.delete(db_route)
    db.commit()
    return {"message": "Route deleted"}