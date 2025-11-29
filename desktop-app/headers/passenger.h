#pragma once

#include "person.h"
#include "ticket.h"
#include <QList>

class Passenger : public Person {
private:
    QString passportNumber;
    QList<Ticket> tickets;

public:
    Passenger(const QString& firstName, const QString& lastName, const QString& middleName,
              const QDate& birthDate, const QString& phoneNumber, const QString& email,
              const QString& passportNumber);

    QString getPassportNumber() const;
    QList<Ticket> getTickets() const;

    void setPassportNumber(const QString& passportNumber);

    void addTicket(const Ticket& ticket);
    bool removeTicket(const QString& ticketNumber);
    Ticket findTicket(const QString& ticketNumber) const;
    int getTicketsCount() const;
    double getTotalSpent() const;

    QString getRole() const override;
    void displayInfo() const override;
    QString toString() const override;
};