#include "schedule.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <algorithm>

Schedule::Schedule() {}

Schedule::Schedule(const QList<Route>& initialRoutes)
    : routes(initialRoutes) {}

void Schedule::addRoute(const Route& route) {
    routes.append(route);
}

bool Schedule::removeRoute(const QString& pathNumber) {
    auto it = std::remove_if(routes.begin(), routes.end(),
        [&pathNumber](const Route& route) {
            return route.getPathNumber() == pathNumber;
        });
    
    if (it != routes.end()) {
        routes.erase(it, routes.end());
        return true;
    }
    return false;
}

QList<Route> Schedule::findRoutesByNumber(const QString& pathNumber) const {
    QList<Route> result;
    for (const auto& route : routes) {
        if (route.getPathNumber() == pathNumber) {
            result.append(route);
        }
    }
    return result;
}

QList<Route> Schedule::findRoutesByDeparture(const QString& departurePlace) const {
    QList<Route> result;
    for (const auto& route : routes) {
        if (route.getDeparturePlace().getStopName().contains(departurePlace, Qt::CaseInsensitive)) {
            result.append(route);
        }
    }
    return result;
}

QList<Route> Schedule::findRoutesByDestination(const QString& destinationPlace) const {
    QList<Route> result;
    for (const auto& route : routes) {
        if (route.getDestinationPlace().getStopName().contains(destinationPlace, Qt::CaseInsensitive)) {
            result.append(route);
        }
    }
    return result;
}

QList<Route> Schedule::findRoutesByDirection(const QString& departurePlace, const QString& destinationPlace) const {
    QList<Route> result;
    for (const auto& route : routes) {
        if (route.getDeparturePlace().getStopName().contains(departurePlace, Qt::CaseInsensitive) &&
            route.getDestinationPlace().getStopName().contains(destinationPlace, Qt::CaseInsensitive)) {
            result.append(route);
        }
    }
    return result;
}

QList<Route> Schedule::getAllRoutes() const {
    return routes;
}

void Schedule::clear() {
    routes.clear();
}

bool Schedule::isEmpty() const {
    return routes.isEmpty();
}

int Schedule::count() const {
    return routes.count();
}

bool Schedule::saveToFile(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    for (const auto& route : routes) {
        out << route.getPathNumber() << ";"
            << route.getPlatformNumber() << ";"
            << route.getDeparturePlace().getStopName() << ";"
            << route.getDestinationPlace().getStopName() << ";"
            << route.getDepartureDate().toString("dd.MM.yyyy") << ";"
            << route.getDepartureTime().toString("hh:mm") << ";"
            << route.getDestinationTime().toString("hh:mm") << ";"
            << QString::number(route.getPrice()) << ";"
            << QString::number(route.getSeats()) << "\n";
    }
    
    file.close();
    return true;
}

bool Schedule::loadFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    routes.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(";");
        
        if (parts.size() == 7) {
            Route route(
                parts[0],
                parts[1],
                QDate::fromString(parts[3], "dd.MM.yyyy"),
                QTime::fromString(parts[4], "hh:mm"),
                QTime::fromString(parts[5], "hh:mm"),
                parts[6].toInt(),
                parts[7].toInt(),
                Path()
            );
            routes.append(route);
        }
    }
    
    file.close();
    return true;
}

void Schedule::displayAll() const {
    if (routes.isEmpty()) {
        qDebug() << "Расписание пустое";
        return;
    }
    
    for (const auto& route : routes) {
        route.displayInfo();
        qDebug() << "---";
    }
}

QString Schedule::toString() const {
    if (routes.isEmpty()) {
        return "Расписание пустое";
    }
    
    QString result;
    for (const auto& route : routes) {
        result += route.toString() + "\n";
    }
    return result;
}