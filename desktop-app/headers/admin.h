#pragma once

#include "passenger.h"
#include "dispatcher.h"
#include "cashier.h"
#include <QList>

class Admin : public Person {
private:
    QString adminId;
    QString accessLevel;
    QList<Passenger> allPassengers;
    QList<Dispatcher> allDispatchers;
    QList<Cashier> allCashiers;

public:
    Admin(const QString& firstName, const QString& lastName, const QString& middleName,
          const QDate& birthDate, const QString& phoneNumber, const QString& email,
          const QString& adminId, const QString& accessLevel);

    // Геттеры
    QString getAdminId() const;
    QString getAccessLevel() const;

    // Сеттеры
    void setAdminId(const QString& adminId);
    void setAccessLevel(const QString& accessLevel);

    void addPassenger(const Passenger& passenger);
    bool removePassenger(const QString& passportNumber);
    
    void addDispatcher(const Dispatcher& dispatcher);
    bool removeDispatcher(const QString& employeeId);
    
    void addCashier(const Cashier& cashier);
    bool removeCashier(const QString& employeeId);
    
    int getTotalUsersCount() const;
    void generateReport() const;

    QString getRole() const override;
    void displayInfo() const override;
    QString toString() const override;
};