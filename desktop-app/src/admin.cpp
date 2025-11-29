#include "admin.h"

Admin::Admin(const QString& firstName, const QString& lastName, const QString& middleName,
             const QDate& birthDate, const QString& phoneNumber, const QString& email,
             const QString& adminId, const QString& accessLevel)
    : Person(firstName, lastName, middleName, birthDate, phoneNumber, email),
      adminId(adminId), accessLevel(accessLevel)
{
}

// Геттеры
QString Admin::getAdminId() const {
    return adminId;
}

QString Admin::getAccessLevel() const {
    return accessLevel;
}

// Сеттеры
void Admin::setAdminId(const QString& adminId) {
    this->adminId = adminId;
}

void Admin::setAccessLevel(const QString& accessLevel) {
    this->accessLevel = accessLevel;
}

void Admin::addPassenger(const Passenger& passenger) {
    allPassengers.append(passenger);
}

bool Admin::removePassenger(const QString& passportNumber) {
    for (int i = 0; i < allPassengers.size(); ++i) {
        if (allPassengers[i].getPassportNumber() == passportNumber) {
            allPassengers.removeAt(i);
            return true;
        }
    }
    return false;
}

void Admin::addDispatcher(const Dispatcher& dispatcher) {
    allDispatchers.append(dispatcher);
}

bool Admin::removeDispatcher(const QString& employeeId) {
    for (int i = 0; i < allDispatchers.size(); ++i) {
        if (allDispatchers[i].getEmployeeId() == employeeId) {
            allDispatchers.removeAt(i);
            return true;
        }
    }
    return false;
}

void Admin::addCashier(const Cashier& cashier) {
    allCashiers.append(cashier);
}

bool Admin::removeCashier(const QString& employeeId) {
    for (int i = 0; i < allCashiers.size(); ++i) {
        if (allCashiers[i].getEmployeeId() == employeeId) {
            allCashiers.removeAt(i);
            return true;
        }
    }
    return false;
}

int Admin::getTotalUsersCount() const {
    return allPassengers.size() + allDispatchers.size() + allCashiers.size();
}

void Admin::generateReport() const {
    qDebug() << "=== ОТЧЕТ АДМИНИСТРАТОРА ===";
    qDebug() << "Всего пассажиров:" << allPassengers.size();
    qDebug() << "Всего диспетчеров:" << allDispatchers.size();
    qDebug() << "Всего кассиров:" << allCashiers.size();
    qDebug() << "Общее количество пользователей:" << getTotalUsersCount();
}

QString Admin::getRole() const {
    return "Admin";
}

void Admin::displayInfo() const {
    qDebug() << "Администратор:" << getFullName()
             << "\nID администратора:" << adminId
             << "\nУровень доступа:" << accessLevel
             << "\nУправление:"
             << "\n  - Пассажиров:" << allPassengers.size()
             << "\n  - Диспетчеров:" << allDispatchers.size()
             << "\n  - Кассиров:" << allCashiers.size();
}

QString Admin::toString() const {
    return QString("Администратор: %1 | ID: %2 | Уровень: %3 | Пользователей: %4")
        .arg(getFullName())
        .arg(adminId)
        .arg(accessLevel)
        .arg(getTotalUsersCount());
}