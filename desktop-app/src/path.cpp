#include "path.h"

Path::Path() {}

Path::Path(const QVector<BusStop>& stops) : stops(stops) {}

void Path::addStop(const BusStop& stop) {
    stops.append(stop);
}

void Path::removeStop(int index) {
    if (index >= 0 && index < stops.size()) {
        stops.remove(index);
    }
}

void Path::clearStops() {
    stops.clear();
}

QVector<BusStop> Path::getStops() const {
    return stops;
}

BusStop Path::getFirstStop() const {
    return stops.isEmpty() ? BusStop() : stops.first();
}

BusStop Path::getLastStop() const {
    return stops.isEmpty() ? BusStop() : stops.last();
}

int Path::getStopCount() const {
    return stops.size();
}