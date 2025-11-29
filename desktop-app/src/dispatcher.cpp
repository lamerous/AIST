#include "dispatcher.h"

Dispatcher::Dispatcher(const QString& firstName, const QString& lastName, const QString& middleName,
                       const QDate& birthDate, const QString& phoneNumber, const QString& email,
                       const QString& employeeId, const QString& department)
    : Person(firstName, lastName, middleName, birthDate, phoneNumber, email),
      employeeId(employeeId), department(department)
{
}

// Геттеры
QString Dispatcher::getEmployeeId() const {
    return employeeId;
}

QString Dispatcher::getDepartment() const {
    return department;
}

QList<Route> Dispatcher::getManagedRoutes() const {
    return managedRoutes;
}

// Сеттеры
void Dispatcher::setEmployeeId(const QString& employeeId) {
    this->employeeId = employeeId;
}

void Dispatcher::setDepartment(const QString& department) {
    this->department = department;
}

void Dispatcher::addRoute(const Route& route) {
    managedRoutes.append(route);
}

bool Dispatcher::removeRoute(const QString& routeNumber) {
    for (int i = 0; i < managedRoutes.size(); ++i) {
        if (managedRoutes[i].getRouteNumber() == routeNumber) {
            managedRoutes.removeAt(i);
            return true;
        }
    }
    return false;
}

Route Dispatcher::findRoute(const QString& routeNumber) const {
    for (const Route& route : managedRoutes) {
        if (route.getRouteNumber() == routeNumber) {
            return route;
        }
    }
    return Route();
}

void Dispatcher::updateRouteSeats(const QString& routeNumber, int seats) {
    for (Route& route : managedRoutes) {
        if (route.getRouteNumber() == routeNumber) {
            route.setSeats(seats);
            break;
        }
    }
}

int Dispatcher::getRoutesCount() const {
    return managedRoutes.size();
}

QString Dispatcher::getRole() const {
    return "Dispatcher";
}

void Dispatcher::displayInfo() const {
    qDebug() << "Диспетчер:" << getFullName()
             << "\nID сотрудника:" << employeeId
             << "\nОтдел:" << department
             << "\nУправляемых маршрутов:" << managedRoutes.size()
             << "\nТелефон:" << phoneNumber;
}

QString Dispatcher::toString() const {
    return QString("Диспетчер: %1 | ID: %2 | Отдел: %3 | Маршрутов: %4")
        .arg(getFullName())
        .arg(employeeId)
        .arg(department)
        .arg(managedRoutes.size());
}