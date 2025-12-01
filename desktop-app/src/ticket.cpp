#include "ticket.h"

Ticket::Ticket()
    : route(Route()), passengerName(""), ticketNumber(""), seatNumber(""), 
      purchaseDate(QDate::currentDate()), actualPrice(0)
{
}

Ticket::Ticket(const Route& route, const QString& passengerName, const QString& ticketNumber,
               const QString& seatNumber, const QDate& purchaseDate, float actualPrice)
    : route(route), passengerName(passengerName), ticketNumber(ticketNumber),
      seatNumber(seatNumber), purchaseDate(purchaseDate), actualPrice(actualPrice)
{
}

Route Ticket::getRoute() const {
    return route;
}

QString Ticket::getPassengerName() const {
    return passengerName;
}

QString Ticket::getTicketNumber() const {
    return ticketNumber;
}

QString Ticket::getSeatNumber() const {
    return seatNumber;
}

QDate Ticket::getPurchaseDate() const {
    return purchaseDate;
}

float Ticket::getActualPrice() const {
    return actualPrice;
}


void Ticket::setRoute(const Route& route) {
    this->route = route;
}

void Ticket::setPassengerName(const QString& name) {
    passengerName = name;
}

void Ticket::setTicketNumber(const QString& number) {
    ticketNumber = number;
}

void Ticket::setSeatNumber(const QString& seat) {
    seatNumber = seat;
}

void Ticket::setPurchaseDate(const QDate& date) {
    purchaseDate = date;
}

void Ticket::setActualPrice(float price) {
    actualPrice = price;
}

void Ticket::displayInfo() const {
    qDebug() << "Билет №" << ticketNumber
             << "\nПассажир:" << passengerName
             << "\nМесто:" << seatNumber
             << "\nДата покупки:" << purchaseDate.toString("dd.MM.yyyy")
             << "\nСтоимость:" << actualPrice << "руб.";
    route.displayInfo();
}

QString Ticket::toString() const {
    return QString("Билет №%1 | Пассажир: %2 | Место: %3 | Куплен: %4 | Цена: %5 руб.\n%6")
        .arg(ticketNumber)
        .arg(passengerName)
        .arg(seatNumber)
        .arg(purchaseDate.toString("dd.MM.yyyy"))
        .arg(actualPrice)
        .arg(route.toString());
}

bool Ticket::isValid() const {
    return !ticketNumber.isEmpty() && 
           !passengerName.isEmpty() && 
           purchaseDate <= route.getDepartureDate() && 
           actualPrice >= 0;
}

QDebug operator<<(QDebug debug, const Ticket& ticket) {
    QDebugStateSaver saver(debug);
    debug.nospace() << "Ticket(" 
                    << "№" << ticket.ticketNumber
                    << ", passenger:" << ticket.passengerName
                    << ", seat:" << ticket.seatNumber
                    << ", purchased:" << ticket.purchaseDate.toString("dd.MM.yyyy")
                    << ", price:" << ticket.actualPrice << " руб."
                    << ", route:" << ticket.route.getRouteNumber()
                    << ")";
    return debug;
}