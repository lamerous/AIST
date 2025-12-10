#include "person.h"
#include <QDate>

Person::Person(const QString& firstName, const QString& lastName, const QString& middleName,
               const QDate& birthDate, const QString& phoneNumber, const QString& email)
    : firstName(firstName), lastName(lastName), middleName(middleName),
      birthDate(birthDate), phoneNumber(phoneNumber), email(email)
{
}

QString Person::getFirstName() const {
    return firstName;
}

QString Person::getLastName() const {
    return lastName;
}

QString Person::getMiddleName() const {
    return middleName;
}

QString Person::getFullName() const {
    return lastName + " " + firstName + " " + middleName;
}

QDate Person::getBirthDate() const {
    return birthDate;
}

QString Person::getPhoneNumber() const {
    return phoneNumber;
}

QString Person::getEmail() const {
    return email;
}

void Person::setFirstName(const QString& firstName) {
    this->firstName = firstName;
}

void Person::setLastName(const QString& lastName) {
    this->lastName = lastName;
}

void Person::setMiddleName(const QString& middleName) {
    this->middleName = middleName;
}

void Person::setBirthDate(const QDate& birthDate) {
    this->birthDate = birthDate;
}

void Person::setPhoneNumber(const QString& phoneNumber) {
    this->phoneNumber = phoneNumber;
}

void Person::setEmail(const QString& email) {
    this->email = email;
}

int Person::getAge() const {
    QDate currentDate = QDate::currentDate();
    int age = currentDate.year() - birthDate.year();
    
    if (birthDate.month() > currentDate.month() || 
        (birthDate.month() == currentDate.month() && birthDate.day() > currentDate.day())) {
        age--;
    }
    
    return age;
}

bool Person::isValid() const {
    return !firstName.isEmpty() && 
           !lastName.isEmpty() && 
           birthDate.isValid() && 
           birthDate < QDate::currentDate() &&
           !phoneNumber.isEmpty();
}

void Person::displayInfo() const {
    qDebug() << getFullName() << ", возраст:" << getAge() 
             << ", тел:" << phoneNumber << ", email:" << email;
}

QString Person::toString() const {
    return QString("%1 %2 лет, тел: %3, email: %4")
        .arg(getFullName())
        .arg(getAge())
        .arg(phoneNumber)
        .arg(email);
}

QDebug operator<<(QDebug debug, const Person& person) {
    QDebugStateSaver saver(debug);
    debug.nospace() << person.toString();
    return debug;
}