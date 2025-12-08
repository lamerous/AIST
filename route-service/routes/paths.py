from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
import models, schemas
from database import get_db

router = APIRouter()

@router.post("/", response_model=schemas.Path)
def create_path(path: schemas.PathCreate, db: Session = Depends(get_db)):
    db_path = models.Path(**path.dict())
    db.add(db_path)
    db.commit()
    db.refresh(db_path)
    return db_path

@router.get("/", response_model=list[schemas.Path])
def read_paths(skip: int = 0, limit: int = 100, search: str = None, db: Session = Depends(get_db)):
    # paths = db.query(models.Path).offset(skip).limit(limit).all()

    query = db.query(models.Path)
    
    if search:
        query = query.filter(
            models.Path.route_number.ilike(f"%{search}%"),
        )
    
    paths = query.offset(skip).limit(limit).all()

    return paths

@router.get("/{path_id}", response_model=schemas.Path)
def read_path(pathNumber: str, db: Session = Depends(get_db)):
    path = db.query(models.Path).filter(models.Path.route_number == pathNumber).first()
    if path is None:
        raise HTTPException(status_code=404, detail="Path not found")
    return path

@router.put("/{path_id}", response_model=schemas.Path)
def update_path(path_id: int, path: schemas.PathCreate, db: Session = Depends(get_db)):
    db_path = db.query(models.Path).filter(models.Path.id == path_id).first()
    if db_path is None:
        raise HTTPException(status_code=404, detail="Path not found")
    for key, value in path.dict().items():
        setattr(db_path, key, value)
    db.commit()
    db.refresh(db_path)
    return db_path

@router.delete("/{path_id}")
def delete_path(path_id: int, db: Session = Depends(get_db)):
    db_path = db.query(models.Path).filter(models.Path.id == path_id).first()
    if db_path is None:
        raise HTTPException(status_code=404, detail="Path not found")
    db.delete(db_path)
    db.commit()
    return {"message": "Path deleted"}