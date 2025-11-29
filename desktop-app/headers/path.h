#pragma once

#include "busstop.h"
#include <QVector>

class Path {
private:
    QVector<BusStop> stops;

public:
    Path();
    Path(const QVector<BusStop>& stops);

    void addStop(const BusStop& stop);
    void removeStop(int index);
    void clearStops();
    
    QVector<BusStop> getStops() const;
    BusStop getFirstStop() const;
    BusStop getLastStop() const;
    int getStopCount() const;
};