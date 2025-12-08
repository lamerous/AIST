#pragma once

#include <QString>
#include <QDate>
#include <QTime>
#include <QDebug>
#include "path.h"

class Route {
private:
    int routeId;
    QString pathNumber;
    QString platformNumber;
    QDate departureDate;
    QTime departureTime;
    QTime destinationTime;
    int routePrice;
    int routeSeats;
    Path path;

public:
    Route();
    Route(const QString& number, const QString& platform, 
          const QDate& date, const QTime& depTime, const QTime& destTime, 
          const int price, const int seats);
    Route(const QString& number, const QString& platform, 
          const QDate& date, const QTime& depTime, const QTime& destTime, 
          const int price, const int seats, const Path& path);

    QString getPathNumber() const;
    QString getPlatformNumber() const;
    BusStop getDeparturePlace() const;
    BusStop getDestinationPlace() const;
    QDate getDepartureDate() const;
    QTime getDepartureTime() const;
    QTime getDestinationTime() const;
    int getPrice() const;
    int getSeats() const;
    Path getPath() const;
    int getId() const;
    bool hasPath() const;

    void setPathNumber(const QString& pathNumber);
    void setPlatformNumber(const QString& platform);
    void setDepartureDate(const QDate& date);
    void setDepartureTime(const QTime& time);
    void setDestinationTime(const QTime& time);
    void setPrice(const int price);
    void setSeats(const int seats);
    void setPath(const Path& path);
    void setId(int id);

    void displayInfo() const;
    QString toString() const;
    friend QDebug operator<<(QDebug debug, const Route& route);
};