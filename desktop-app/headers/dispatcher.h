#pragma once

#include "person.h"
#include "route.h"
#include <QList>

class Dispatcher : public Person {
private:
    QString employeeId;
    QString department;
    QList<Route> managedRoutes;

public:
    Dispatcher(const QString& firstName, const QString& lastName, const QString& middleName,
               const QDate& birthDate, const QString& phoneNumber, const QString& email,
               const QString& employeeId, const QString& department);

    QString getEmployeeId() const;
    QString getDepartment() const;
    QList<Route> getManagedRoutes() const;

    void setEmployeeId(const QString& employeeId);
    void setDepartment(const QString& department);

    void addRoute(const Route& route);
    bool removeRoute(const QString& routeNumber);
    Route findRoute(const QString& routeNumber) const;
    void updateRouteSeats(const QString& routeNumber, int seats);
    int getRoutesCount() const;

    QString getRole() const override;
    void displayInfo() const override;
    QString toString() const override;
};