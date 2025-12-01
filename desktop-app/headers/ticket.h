#pragma once

#include <QString>
#include <QDate>
#include <QTime>
#include <QDebug>
#include "route.h"

class Ticket {
private:
    Route route;
    QString passengerName;
    QString ticketNumber;
    QString seatNumber;
    QDate purchaseDate;

public:
    Ticket();
    Ticket(const Route& route, const QString& passengerName, const QString& ticketNumber, 
           const QString& seatNumber, const QDate& purchaseDate);

    Route getRoute() const;
    QString getPassengerName() const;
    QString getTicketNumber() const;
    QString getSeatNumber() const;
    QDate getPurchaseDate() const;

    void setRoute(const Route& route);
    void setPassengerName(const QString& name);
    void setTicketNumber(const QString& number);
    void setSeatNumber(const QString& seat);
    void setPurchaseDate(const QDate& date);

    void displayInfo() const;
    QString toString() const;
    bool isValid() const;

    friend QDebug operator<<(QDebug debug, const Ticket& ticket);
};