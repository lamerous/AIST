#include "path.h"

int Path::counter = 0;

Path::Path() {}

Path::Path(QString pathNumber) : pathNumber(pathNumber) {}

Path::Path(QString pathNumber, const QVector<BusStop>& stops) : pathNumber(pathNumber), pathStops(stops) {}

void Path::addStop(const BusStop& stop) {
    pathStops.append(stop);
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

int Path::getId() const{
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