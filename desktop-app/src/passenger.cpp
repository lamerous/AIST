#include "passenger.h"

Passenger::Passenger(const QString& firstName, const QString& lastName, const QString& middleName,
                     const QDate& birthDate, const QString& phoneNumber, const QString& email,
                     const QString& passportNumber)
    : Person(firstName, lastName, middleName, birthDate, phoneNumber, email),
      passportNumber(passportNumber)
{
}

// Геттеры
QString Passenger::getPassportNumber() const {
    return passportNumber;
}

QList<Ticket> Passenger::getTickets() const {
    return tickets;
}

// Сеттеры
void Passenger::setPassportNumber(const QString& passportNumber) {
    this->passportNumber = passportNumber;
}

void Passenger::addTicket(const Ticket& ticket) {
    tickets.append(ticket);
}

bool Passenger::removeTicket(const QString& ticketNumber) {
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].getTicketNumber() == ticketNumber) {
            tickets.removeAt(i);
            return true;
        }
    }
    return false;
}

Ticket Passenger::findTicket(const QString& ticketNumber) const {
    for (const Ticket& ticket : tickets) {
        if (ticket.getTicketNumber() == ticketNumber) {
            return ticket;
        }
    }
    return Ticket();
}

int Passenger::getTicketsCount() const {
    return tickets.size();
}

double Passenger::getTotalSpent() const {
    double total = 0;
    for (const Ticket& ticket : tickets) {
        total += ticket.getActualPrice();
    }
    return total;
}

QString Passenger::getRole() const {
    return "Passenger";
}

void Passenger::displayInfo() const {
    qDebug() << "Пассажир:" << getFullName()
             << "\nПаспорт:" << passportNumber
             << "\nВозраст:" << getAge()
             << "\nТелефон:" << phoneNumber
             << "\nEmail:" << email
             << "\nКоличество билетов:" << tickets.size()
             << "\nОбщие расходы:" << getTotalSpent() << "руб.";
}

QString Passenger::toString() const {
    return QString("Пассажир: %1 | Паспорт: %2 | Билетов: %3 | Телефон: %4")
        .arg(getFullName())
        .arg(passportNumber)
        .arg(tickets.size())
        .arg(phoneNumber);
}