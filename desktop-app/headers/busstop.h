#pragma once

#include <QString>

class BusStop {
private:
    static int counter;
    int stopId;
    QString stopName;
    QString stopAddress;

public:
    BusStop();
    BusStop(const QString& name, const QString& address);
    BusStop(const int id, const QString& name, const QString& address);

    int getStopId() const;
    QString getStopName() const;
    QString getStopAddress() const;

    void setStopName(QString stopName);
    void setStopAdress(QString stopAddress);
};