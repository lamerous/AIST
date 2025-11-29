#include "cashier.h"
#include <QUuid>

Cashier::Cashier(const QString& firstName, const QString& lastName, const QString& middleName,
                 const QDate& birthDate, const QString& phoneNumber, const QString& email,
                 const QString& employeeId, int workstationNumber)
    : Person(firstName, lastName, middleName, birthDate, phoneNumber, email),
      employeeId(employeeId), workstationNumber(workstationNumber), totalSales(0)
{
}

// Геттеры
QString Cashier::getEmployeeId() const {
    return employeeId;
}

int Cashier::getWorkstationNumber() const {
    return workstationNumber;
}

double Cashier::getTotalSales() const {
    return totalSales;
}

QList<Ticket> Cashier::getSoldTickets() const {
    return soldTickets;
}

// Сеттеры
void Cashier::setEmployeeId(const QString& employeeId) {
    this->employeeId = employeeId;
}

void Cashier::setWorkstationNumber(int workstationNumber) {
    this->workstationNumber = workstationNumber;
}

Ticket Cashier::sellTicket(const Route& route, const QString& passengerName, 
                          const QString& seatNumber, int actualPrice) {
    QString ticketNumber = QUuid::createUuid().toString().mid(1, 8).toUpper();
    Ticket ticket(route, passengerName, ticketNumber, seatNumber, QDate::currentDate(), actualPrice);
    
    soldTickets.append(ticket);
    totalSales += actualPrice;
    
    return ticket;
}

bool Cashier::refundTicket(const QString& ticketNumber) {
    for (int i = 0; i < soldTickets.size(); ++i) {
        if (soldTickets[i].getTicketNumber() == ticketNumber) {
            totalSales -= soldTickets[i].getActualPrice();
            soldTickets.removeAt(i);
            return true;
        }
    }
    return false;
}

int Cashier::getSoldTicketsCount() const {
    return soldTickets.size();
}

double Cashier::getAverageTicketPrice() const {
    if (soldTickets.isEmpty()) return 0;
    return totalSales / soldTickets.size();
}

QString Cashier::getRole() const {
    return "Cashier";
}

void Cashier::displayInfo() const {
    qDebug() << "Кассир:" << getFullName()
             << "\nID сотрудника:" << employeeId
             << "\nРабочее место:" << workstationNumber
             << "\nПродано билетов:" << soldTickets.size()
             << "\nОбщая выручка:" << totalSales << "руб."
             << "\nСредняя цена:" << getAverageTicketPrice() << "руб.";
}

QString Cashier::toString() const {
    return QString("Кассир: %1 | ID: %2 | Место: %3 | Продаж: %4 | Выручка: %5 руб.")
        .arg(getFullName())
        .arg(employeeId)
        .arg(workstationNumber)
        .arg(soldTickets.size())
        .arg(totalSales);
}