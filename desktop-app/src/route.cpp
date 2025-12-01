#include "route.h"

Route::Route()
    : routeNumber(""), platformNumber(""), departureDate(QDate::currentDate()), 
      departureTime(QTime::currentTime()), destinationTime(QTime::currentTime()), 
      routePrice(0), routeSeats(0), path(Path())
{
}

Route::Route(const QString& number, const QString& platform, 
             const QDate& date, const QTime& depTime, const QTime& destTime,
             const int price, const int seats, const Path& path)
    : routeNumber(number), platformNumber(platform),
      departureDate(date), departureTime(depTime), destinationTime(destTime),
      routePrice(price), routeSeats(seats), path(path)
{
}

QString Route::getRouteNumber() const {
    return routeNumber;
}

QString Route::getPlatformNumber() const {
    return platformNumber;
}

BusStop Route::getDeparturePlace() const {
    return path.getFirstStop();
}

BusStop Route::getDestinationPlace() const {
    return path.getLastStop();
}

QDate Route::getDepartureDate() const {
    return departureDate;
}

QTime Route::getDepartureTime() const {
    return departureTime;
}

QTime Route::getDestinationTime() const {
    return destinationTime;
}

int Route::getPrice() const {
    return routePrice;
}


int Route::getSeats() const {
    return routeSeats;
}

Path Route::getPath() const {
    return path;
}


// Сеттеры
void Route::setRouteNumber(const QString& number) {
    routeNumber = number;
}

void Route::setPlatformNumber(const QString& platform) {
    platformNumber = platform;
}

void Route::setDepartureDate(const QDate& date) {
    departureDate = date;
}

void Route::setDepartureTime(const QTime& time) {
    departureTime = time;
}

void Route::setDestinationTime(const QTime& time) {
    destinationTime = time;
}

void Route::setPrice(const int price) {
    routePrice = price;
}

void Route::setSeats(const int seats) {
    routeSeats = seats;
}

void Route::setPath(const Path& path) {
    this->path = path;
}

void Route::displayInfo() const {
    qDebug() << "Маршрут №" << routeNumber
             << "\nПлатформа:" << platformNumber
             << "\nОтправление:" << path.getFirstStop().getStopName()
            << departureDate.toString("dd.MM.yyyy") << departureTime.toString("hh:mm")
             << "\nПрибытие:" << path.getLastStop().getStopName() << destinationTime.toString("hh:mm");
}

QString Route::toString() const {
    return QString("Маршрут №%1 | Платформа: %2 | %3 -> %4 | %5 %6 - %7 | %8 руб. | %9 мест")
        .arg(routeNumber)
        .arg(platformNumber)
        .arg(path.getFirstStop().getStopName())
        .arg(path.getLastStop().getStopName())
        .arg(departureDate.toString("dd.MM.yyyy"))
        .arg(departureTime.toString("hh:mm"))
        .arg(destinationTime.toString("hh:mm"))
        .arg(routePrice)
        .arg(routeSeats);
}

// Перегрузка оператора вывода
QDebug operator<<(QDebug debug, const Route& route) {
    QDebugStateSaver saver(debug);
    debug.nospace() << "Route(" 
                    << "№" << route.routeNumber
                    << ", platform:" << route.platformNumber
                    << ", " << route.getDeparturePlace().getStopName()
                    << "->" << route.getDestinationPlace().getStopName()
                    << ", " << route.departureDate.toString("dd.MM.yyyy")
                    << " " << route.departureTime.toString("hh:mm") << "-" << route.destinationTime.toString("hh:mm")
                    << " " << route.routePrice << " руб. " << route.routeSeats
                    << ")";
    return debug;
}