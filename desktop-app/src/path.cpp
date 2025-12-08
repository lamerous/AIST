#include "path.h"

int Path::counter = 0;

Path::Path() {
    counter++;
    pathId = counter;
}

Path::Path(const QString pathNumber) : pathNumber(pathNumber) {
    counter++;
    pathId = counter;
}

Path::Path(int pathId, const QString pathNumber) : pathId(pathId), pathNumber(pathNumber) {
}

Path::Path(const QString pathNumber, const QVector<BusStop> &stops) : pathNumber(pathNumber), pathStops(stops) {
    counter++;
    pathId = counter;
}

Path::Path(int pathId, const QString pathNumber, const QVector<BusStop> &stops) : pathId(pathId), pathNumber(pathNumber), pathStops(stops) {
}

void Path::addStop(const BusStop &stop) {
    pathStops.append(stop);
}

void Path::removeStopById(int stopId) {
    for (int i = 0; i < pathStops.size(); i++){
        if (pathStops[i].getStopId() == stopId) {
            pathStops.remove(i);
        }
    }
}

void Path::removeStop(int index) {
    if (index >= 0 && index < pathStops.size()) {
        pathStops.remove(index);
    }
}

void Path::clearStops() {
    pathStops.clear();
}

void Path::setNumber(const QString number) {
    pathNumber = number;
}

int Path::getPathId() const{
    return pathId;
}

QString Path::getNumber() const {
    return pathNumber;
}

QVector<BusStop> Path::getStops() const {
    return pathStops;
}

BusStop Path::getFirstStop() const {
    return pathStops.isEmpty() ? BusStop() : pathStops.first();
}

BusStop Path::getLastStop() const {
    return pathStops.isEmpty() ? BusStop() : pathStops.last();
}

int Path::getStopCount() const {
    return pathStops.size();
}