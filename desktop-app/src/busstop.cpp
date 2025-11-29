#include "busstop.h"

int BusStop::counter = 0;

BusStop::BusStop() : stopName(""), stopAddress("")
{
    counter++;
    stopId = counter;
}

BusStop::BusStop(const QString& name, const QString& address) : 
    stopName(name),
    stopAddress(address)
{
    counter++;
    stopId = counter;
}

BusStop::BusStop(const int id, const QString& name, const QString& address) :
    stopName(name),
    stopAddress(address)
{
    counter++;
    stopId = id;
}

int BusStop::getStopId() const {
    return stopId;
}

QString BusStop::getStopName() const {
    return stopName;
}

QString BusStop::getStopAddress() const {
    return stopAddress;
}

void BusStop::setStopName(QString name) {
    stopName = name;
}

void BusStop::setStopAdress(QString address) {
    stopAddress = address;
}