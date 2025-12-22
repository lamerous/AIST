from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
from typing import Optional
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
    start_stop: Optional[str] = None,
    end_stop: Optional[str] = None,
    db: Session = Depends(get_db)
):
    """
    Получить список маршрутов с возможностью фильтрации по начальной и конечной остановке.
    start_stop: название начальной остановки
    end_stop: название конечной остановки
    """
    # Сначала получаем все маршруты с путями
    routes_with_paths = (
        db.query(models.Route, models.Path)
        .join(
            models.Path, 
            models.Route.path_number == models.Path.route_number,
            isouter=True
        )
        .all()
    )
    
    # Фильтруем по остановкам если указаны
    filtered_results = []
    
    for route, path in routes_with_paths:
        if not path:
            # Если путь отсутствует, пропускаем маршрут
            continue
            
        stops_array = path.stops_array.get("stops", []) if isinstance(path.stops_array, dict) else []
        
        # Получаем названия остановок
        stop_names = []
        for stop in stops_array:
            if isinstance(stop, dict):
                stop_names.append(stop.get("name", ""))
            elif isinstance(stop, int):
                # Если остановка представлена как ID, получаем её название из базы
                stop_obj = db.query(models.Stop).filter(models.Stop.id == stop).first()
                if stop_obj:
                    stop_names.append(stop_obj.stop_name)
        
        # Проверяем условия фильтрации
        meets_start_condition = True
        meets_end_condition = True
        
        if start_stop:
            meets_start_condition = start_stop in stop_names
        
        if end_stop:
            meets_end_condition = end_stop in stop_names
        
        # Если оба условия выполнены, добавляем маршрут
        if meets_start_condition and meets_end_condition:
            route_data = schemas.Route.from_orm(route)
            route_data.path = {
                "id": path.id,
                "route_number": path.route_number,
                "stops_array": path.stops_array
            }
            filtered_results.append(route_data)
    
    # Применяем пагинацию
    result = filtered_results[skip:skip + limit]
    
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