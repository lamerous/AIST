#pragma once

#include <QString>
#include <QDate>
#include <QTime>
#include <QDebug>
#include "route.h"

class Ticket {
private:
    int id = 0;
    int userId = 0;
    Route route;
    QString passengerName;
    QString ticketNumber;
    QString seatNumber;
    QDate purchaseDate;
    float actualPrice;
    QTime departureTime;
    QTime arrivalTime;
    QString startStop;
    QString endStop;

public:
    Ticket();
    Ticket(const Route& route, const QString& passengerName, const QString& ticketNumber, 
           const QString& seatNumber, const QDate& purchaseDate, float actualPrice);

    // Геттеры
    int getId() const;
    int getUserId() const;
    Route getRoute() const;
    QString getPassengerName() const;
    QString getTicketNumber() const;
    QString getSeatNumber() const;
    QDate getPurchaseDate() const;
    float getActualPrice() const;
    QTime getDepartureTime() const;
    QTime getArrivalTime() const;
    QString getStartStop() const;
    QString getEndStop() const;

    // Сеттеры
    void setId(int id);
    void setUserId(int userId);
    void setRoute(const Route& route);
    void setPassengerName(const QString& name);
    void setTicketNumber(const QString& number);
    void setSeatNumber(const QString& seat);
    void setPurchaseDate(const QDate& date);
    void setActualPrice(float price);
    void setDepartureTime(const QTime& time);
    void setArrivalTime(const QTime& time);
    void setStartStop(const QString& stop);
    void setEndStop(const QString& stop);

    // Методы
    void displayInfo() const;
    QString toString() const;
    bool isValid() const;
    
    // Статический метод для создания из JSON
    static Ticket fromJson(const QJsonObject& json);

    friend QDebug operator<<(QDebug debug, const Ticket& ticket);
};