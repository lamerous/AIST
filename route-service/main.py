from fastapi import FastAPI
from routes import stops, paths, routes, tickets
import uvicorn

app = FastAPI(title="Routes DB Microservice")

app.include_router(stops.router, prefix="/api/stops", tags=["stops"])
app.include_router(paths.router, prefix="/api/paths", tags=["paths"])
app.include_router(routes.router, prefix="/api/routes", tags=["routes"])
app.include_router(tickets.router, prefix="/api/tickets", tags=["tickets"])

if __name__ == "__main__":
    uvicorn.run("main:app", host="0.0.0.0", port=8000, reload=True)