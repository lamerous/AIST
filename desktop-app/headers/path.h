#pragma once

#include "busstop.h"
#include <QVector>

class Path {
private:
    static int counter;
    int pathId;
    QString pathNumber;
    QVector<BusStop> pathStops;

public:
    Path();
    Path(const QString pathNumber);
    Path(int pathid, const QString pathNumber);
    Path(const QString pathNumber, const QVector<BusStop>& stops);
    Path(int pathId, const QString pathNumber, const QVector<BusStop> &stops);

    void addStop(const BusStop& stop);
    void removeStop(int index);
    void removeStopById(int stopId);
    void clearStops();
    
    void setNumber(const QString nubmer);

    int getPathId() const;
    QString getNumber() const;
    QVector<BusStop> getStops() const;
    BusStop getFirstStop() const;
    BusStop getLastStop() const;
    int getStopCount() const;
};