#pragma once

#include "person.h"
#include "ticket.h"
#include <QList>

class Cashier : public Person {
private:
    QString employeeId;
    int workstationNumber;
    double totalSales;
    QList<Ticket> soldTickets;

public:
    Cashier(const QString& firstName, const QString& lastName, const QString& middleName,
            const QDate& birthDate, const QString& phoneNumber, const QString& email,
            const QString& employeeId, int workstationNumber);

    // Геттеры
    QString getEmployeeId() const;
    int getWorkstationNumber() const;
    double getTotalSales() const;
    QList<Ticket> getSoldTickets() const;

    // Сеттеры
    void setEmployeeId(const QString& employeeId);
    void setWorkstationNumber(int workstationNumber);

    Ticket sellTicket(const Route& route, const QString& passengerName, 
                     const QString& seatNumber, int actualPrice);
    bool refundTicket(const QString& ticketNumber);
    int getSoldTicketsCount() const;
    double getAverageTicketPrice() const;

    QString getRole() const override;
    void displayInfo() const override;
    QString toString() const override;
};