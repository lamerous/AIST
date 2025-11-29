#pragma once

#include "route.h"
#include <QList>
#include <QString>

class Schedule {
private:
    QList<Route> routes;

public:
    Schedule();
    Schedule(const QList<Route>& initialRoutes);

    void addRoute(const Route& route);
    bool removeRoute(const QString& routeNumber);
    QList<Route> findRoutesByNumber(const QString& routeNumber) const;
    QList<Route> findRoutesByDeparture(const QString& departurePlace) const;
    QList<Route> findRoutesByDestination(const QString& destinationPlace) const;
    QList<Route> findRoutesByDirection(const QString& departurePlace, const QString& destinationPlace) const;
    QList<Route> getAllRoutes() const;
    void clear();
    bool isEmpty() const;
    int count() const;

    bool saveToFile(const QString& filename) const;
    bool loadFromFile(const QString& filename);

    void displayAll() const;
    QString toString() const;
};