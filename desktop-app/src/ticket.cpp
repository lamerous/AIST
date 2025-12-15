#include "ticket.h"
#include <QJsonObject>
#include <QJsonArray>

Ticket::Ticket()
    : route(Route()), passengerName(""), ticketNumber(""), seatNumber(""), 
      purchaseDate(QDate::currentDate()), actualPrice(0),
      departureTime(QTime()), arrivalTime(QTime()), startStop(""), endStop("")
{
}

Ticket::Ticket(const Route& route, const QString& passengerName, const QString& ticketNumber,
               const QString& seatNumber, const QDate& purchaseDate, float actualPrice)
    : route(route), passengerName(passengerName), ticketNumber(ticketNumber),
      seatNumber(seatNumber), purchaseDate(purchaseDate), actualPrice(actualPrice),
      departureTime(QTime()), arrivalTime(QTime()), startStop(""), endStop("")
{
}

// Геттеры
int Ticket::getId() const {
    return id;
}

int Ticket::getUserId() const {
    return userId;
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

QTime Ticket::getDepartureTime() const {
    return departureTime;
}

QTime Ticket::getArrivalTime() const {
    return arrivalTime;
}

QString Ticket::getStartStop() const {
    return startStop;
}

QString Ticket::getEndStop() const {
    return endStop;
}

// Сеттеры
void Ticket::setId(int id) {
    this->id = id;
}

void Ticket::setUserId(int userId) {
    this->userId = userId;
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

void Ticket::setDepartureTime(const QTime& time) {
    departureTime = time;
}

void Ticket::setArrivalTime(const QTime& time) {
    arrivalTime = time;
}

void Ticket::setStartStop(const QString& stop) {
    startStop = stop;
}

void Ticket::setEndStop(const QString& stop) {
    endStop = stop;
}

void Ticket::displayInfo() const {
    qDebug() << "=== Информация о билете ===";
    qDebug() << "ID билета:" << id;
    qDebug() << "ID пользователя:" << userId;
    qDebug() << "Номер билета:" << ticketNumber;
    qDebug() << "Пассажир:" << passengerName;
    qDebug() << "Место:" << seatNumber;
    qDebug() << "Дата покупки:" << purchaseDate.toString("dd.MM.yyyy");
    
    if (!departureTime.isNull()) {
        qDebug() << "Время отправления:" << departureTime.toString("hh:mm");
    }
    
    if (!arrivalTime.isNull()) {
        qDebug() << "Время прибытия:" << arrivalTime.toString("hh:mm");
    }
    
    if (!startStop.isEmpty()) {
        qDebug() << "Начальная остановка:" << startStop;
    }
    
    if (!endStop.isEmpty()) {
        qDebug() << "Конечная остановка:" << endStop;
    }
    
    qDebug() << "\nИнформация о маршруте:";
    route.displayInfo();
    qDebug() << "===========================";
}

QString Ticket::toString() const {
    QString result = QString("Билет №%1 | ID: %2 | Пассажир: %3\n")
        .arg(ticketNumber)
        .arg(id)
        .arg(passengerName);
    
    result += QString("Куплен: %1\n")
        .arg(purchaseDate.toString("dd.MM.yyyy"));
    
    if (!departureTime.isNull() && !arrivalTime.isNull()) {
        result += QString("Время: %1 - %2\n")
            .arg(departureTime.toString("hh:mm"))
            .arg(arrivalTime.toString("hh:mm"));
    }
    
    if (!startStop.isEmpty() && !endStop.isEmpty()) {
        result += QString("Маршрут: %1 → %2\n")
            .arg(startStop)
            .arg(endStop);
    }
    
    return result;
}

bool Ticket::isValid() const {
    return !ticketNumber.isEmpty() && 
           !passengerName.isEmpty() && 
           actualPrice >= 0 &&
           id >= 0 &&
           userId >= 0;
}

Ticket Ticket::fromJson(const QJsonObject& json) {
    Ticket ticket;
    
    if (json.contains("id") && !json["id"].isNull()) {
        ticket.setId(json["id"].toInt());
    }
    
    if (json.contains("user_id") && !json["user_id"].isNull()) {
        ticket.setUserId(json["user_id"].toInt());
    }
    
    if (json.contains("ticket_number") && !json["ticket_number"].isNull()) {
        ticket.setTicketNumber(json["ticket_number"].toString());
    }
    
    if (json.contains("purchase_date") && !json["purchase_date"].isNull()) {
        QDate purchaseDate = QDate::fromString(json["purchase_date"].toString(), "yyyy-MM-dd");
        ticket.setPurchaseDate(purchaseDate);
    }
    
    if (json.contains("purchase_price") && !json["purchase_price"].isNull()) {
        ticket.setActualPrice(json["purchase_price"].toDouble());
    }
    
    if (json.contains("departure_time") && !json["departure_time"].isNull()) {
        QTime departureTime = QTime::fromString(json["departure_time"].toString(), "hh:mm:ss");
        ticket.setDepartureTime(departureTime);
    }
    
    if (json.contains("arrival_time") && !json["arrival_time"].isNull()) {
        QTime arrivalTime = QTime::fromString(json["arrival_time"].toString(), "hh:mm:ss");
        ticket.setArrivalTime(arrivalTime);
    }
    
    if (json.contains("start_stop") && !json["start_stop"].isNull()) {
        ticket.setStartStop(json["start_stop"].toString());
    }
    
    if (json.contains("end_stop") && !json["end_stop"].isNull()) {
        ticket.setEndStop(json["end_stop"].toString());
    }
    
    if (json.contains("route_id") && !json["route_id"].isNull()) {
        Route stubRoute;
        stubRoute.setId(json["route_id"].toInt());
        ticket.setRoute(stubRoute);
    }
    
    ticket.setPassengerName("Пассажир");
    
    ticket.setSeatNumber("A1");
    
    return ticket;
}

QDebug operator<<(QDebug debug, const Ticket& ticket) {
    QDebugStateSaver saver(debug);
    debug.nospace() << "Ticket(" 
                    << "id:" << ticket.id
                    << ", user_id:" << ticket.userId
                    << ", №" << ticket.ticketNumber
                    << ", passenger:" << ticket.passengerName
                    << ", seat:" << ticket.seatNumber
                    << ", purchased:" << ticket.purchaseDate.toString("dd.MM.yyyy")
                    << ", price:" << ticket.actualPrice << " руб.";
    
    if (!ticket.departureTime.isNull() && !ticket.arrivalTime.isNull()) {
        debug.nospace() << ", time:" << ticket.departureTime.toString("hh:mm") 
                        << "-" << ticket.arrivalTime.toString("hh:mm");
    }
    
    if (!ticket.startStop.isEmpty() && !ticket.endStop.isEmpty()) {
        debug.nospace() << ", route:" << ticket.startStop << "→" << ticket.endStop;
    }
    
    debug.nospace() << ", route_id:" << ticket.route.getId() << ")";
    
    return debug;
}