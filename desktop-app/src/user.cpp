#include "user.h"
#include <QFile>
#include <QDir>
#include <QDebug>


User::User() 
    : userId(0), authorized(false)
{
}

User::User(const Person& person, const QString& username, const QString& role, int id)
    : person(person), username(username), userId(id), role(role), authorized(false)
{
    checkCookieFile();
}

User::User(const QString& firstName, const QString& lastName, const QString& middleName,
           const QDate& birthDate, const QString& phoneNumber, const QString& email,
           const QString& username, const QString& role, int id)
    : person(firstName, lastName, middleName, birthDate, phoneNumber, email),
      username(username),
      userId(id),
      role(role),
      authorized(false)
{
    checkCookieFile();
}

User::~User(){
    
}

Person User::getPerson() const {
    return person;
}

QString User::getUsername() const {
    return username;
}

int User::getUserId() const {
    return userId;
}

bool User::isAuthorized() const {
    return authorized;
}

QString User::getRole() const {
    return role;
}

QString User::getCookie() const {
    return cookie;
}

void User::setPerson(const Person& person) {
    this->person = person;
}

void User::setUsername(const QString& username) {
    this->username = username;
}

void User::setAuthorized(bool authorized) {
    this->authorized = authorized;
}

void User::setUserId(int userId) {
    this->userId = userId;
}

void User::setRole(const QString& role) {
    this->role = role;
}

void User::checkCookieFile() {
    authorized = cookieFileExists();

    qDebug() << "AUTHORIZED " << authorized; 
    
    if (authorized) {
        qDebug() << "Cookie файл найден, пользователь авторизован";
    } else {
        qDebug() << "Cookie файл не найден, пользователь не авторизован";
    }
}

bool User::cookieFileExists() {
    QFile cookieFile("cookie");
    bool exists = cookieFile.exists();
    
    if (exists) {
        if (cookieFile.size() == 0) {
            return false;
        }
        if (cookieFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&cookieFile);
            cookie = in.readAll().trimmed();
            cookieFile.close();
            
            return true;
        } else {
            return false;
        }
    }
    return false;
}

QString User::toString() const {
    QString authStatus = authorized ? "Авторизован" : "Не авторизован";
    return QString("Пользователь #%1: %2 (логин: %3), статус: %4")
        .arg(userId)
        .arg(person.getFullName())
        .arg(username)
        .arg(authStatus);
}

void User::displayInfo() const {
    qDebug() << toString();
    person.displayInfo();
}