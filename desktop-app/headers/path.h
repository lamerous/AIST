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
    Path(QString pathNumber);
    Path(QString pathNumber, const QVector<BusStop>& stops);

    void addStop(const BusStop& stop);
    void removeStop(int index);
    void clearStops();
    
    void setNumber(const QString nubmer);

    int getId() const;
    QString getNumber() const;
    QVector<BusStop> getStops() const;
    BusStop getFirstStop() const;
    BusStop getLastStop() const;
    int getStopCount() const;
};