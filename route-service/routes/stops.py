from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
from sqlalchemy import or_
import models, schemas
from database import get_db

router = APIRouter()

@router.post("/", response_model=schemas.Stop)
def create_stop(stop: schemas.StopCreate, db: Session = Depends(get_db)):
    db_stop = models.Stop(**stop.dict())
    db.add(db_stop)
    db.commit()
    db.refresh(db_stop)
    return db_stop

@router.get("/", response_model=list[schemas.Stop])
def read_stops(skip: int = 0, limit: int = 100, search: str = None, db: Session = Depends(get_db)):
    # stops = db.query(models.Stop).offset(skip).limit(limit).all()
    query = db.query(models.Stop)
    
    if search:
        query = query.filter(
            or_(
                models.Stop.stop_name.ilike(f"%{search}%"),
                models.Stop.stop_address.ilike(f"%{search}%")
            )
        )
    if search:
        stops = query.order_by(models.Stop.id.desc()).offset(skip).limit(limit).all()
    else:
        stops = query.offset(skip).limit(limit).all()

    return stops

@router.get("/{stop_id}", response_model=schemas.Stop)
def read_stop(stop_id: int, db: Session = Depends(get_db)):
    stop = db.query(models.Stop).filter(models.Stop.id == stop_id).first()
    if stop is None:
        raise HTTPException(status_code=404, detail="Stop not found")
    return stop

@router.put("/{stop_id}", response_model=schemas.Stop)
def update_stop(stop_id: int, stop: schemas.StopCreate, db: Session = Depends(get_db)):
    db_stop = db.query(models.Stop).filter(models.Stop.id == stop_id).first()
    if db_stop is None:
        raise HTTPException(status_code=404, detail="Stop not found")
    for key, value in stop.dict().items():
        setattr(db_stop, key, value)
    db.commit()
    db.refresh(db_stop)
    return db_stop

@router.delete("/{stop_id}")
def delete_stop(stop_id: int, db: Session = Depends(get_db)):
    db_stop = db.query(models.Stop).filter(models.Stop.id == stop_id).first()
    if db_stop is None:
        raise HTTPException(status_code=404, detail="Stop not found")
    db.delete(db_stop)
    db.commit()
    return {"message": "Stop deleted"}