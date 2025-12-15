#include "orderwidget.h"

#include <QDebug>
#include <QMouseEvent>
#include <QLabel>
#include <QTime>

OrderWidget::OrderWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::OrderWidget)
{
    ui->setupUi(this);
    setupUI();
}

OrderWidget::OrderWidget(const Ticket& ticket, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::OrderWidget())
    , currentTicket(ticket)
{
    ui->setupUi(this);
    setupUI();
    updateOrderData(ticket);
}

OrderWidget::~OrderWidget() {
    delete ui;
}

void OrderWidget::setupUI() {
    this->setObjectName("roundedWidget");
}

void OrderWidget::updateOrderData(const Ticket& ticket) {
    currentTicket = ticket;
    updateUIFromTicket();
}

void OrderWidget::updateOrderData(int orderNumber, 
                                 const QDate& orderDate,
                                 const QTime& departureTime,
                                 const QString& departurePlace,
                                 const QTime& arrivalTime,
                                 const QString& arrivalPlace) {
    Ticket tempTicket;
    tempTicket.setId(orderNumber);
    tempTicket.setPurchaseDate(orderDate);
    tempTicket.setDepartureTime(departureTime);
    tempTicket.setStartStop(departurePlace);
    tempTicket.setArrivalTime(arrivalTime);
    tempTicket.setEndStop(arrivalPlace);
    
    currentTicket = tempTicket;
    updateUIFromTicket();
}

void OrderWidget::updateUIFromTicket() {
    ui->orderNumber->setText(QString::number(currentTicket.getId()));
    
    if (currentTicket.getPurchaseDate().isValid()) {
        ui->orderDate->setText(currentTicket.getPurchaseDate().toString("dd.MM.yyyy"));
    } else {
        ui->orderDate->setText("Не указана");
    }
    
    if (currentTicket.getDepartureTime().isValid()) {
        ui->departureTime->setText(currentTicket.getDepartureTime().toString("hh:mm"));
    } else {
        ui->departureTime->setText("--:--");
    }
    
    if (currentTicket.getArrivalTime().isValid()) {
        ui->arivingTime->setText(currentTicket.getArrivalTime().toString("hh:mm"));
    } else {
        ui->arivingTime->setText("--:--");
    }
    
    ui->departurePlace->setText(currentTicket.getStartStop());
    ui->arivingPlace->setText(currentTicket.getEndStop());
}

Ticket OrderWidget::getTicket() const {
    return currentTicket;
}

int OrderWidget::getOrderNumber() const {
    return currentTicket.getId();
}

QDate OrderWidget::getOrderDate() const {
    return currentTicket.getPurchaseDate();
}

QTime OrderWidget::getDepartureTime() const {
    return currentTicket.getDepartureTime();
}

QString OrderWidget::getDeparturePlace() const {
    return currentTicket.getStartStop();
}

QTime OrderWidget::getArrivalTime() const {
    return currentTicket.getArrivalTime();
}

QString OrderWidget::getArrivalPlace() const {
    return currentTicket.getEndStop();
}

void OrderWidget::on_downloadTicket_clicked() {
    emit downloadTicketClicked(currentTicket);
    qDebug() << "Скачан билет для заказа №" << currentTicket.getId() 
             << "Пассажир:" << currentTicket.getPassengerName();
}

void OrderWidget::on_unfilledPushButton_downloadCheck_clicked() {
    emit downloadCheckClicked(currentTicket);
    qDebug() << "Скачан чек для заказа №" << currentTicket.getId() 
             << "Стоимость:" << currentTicket.getActualPrice();
}