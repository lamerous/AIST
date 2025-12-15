#pragma once

#include <QWidget>
#include <QTime>
#include <QDate>
#include "ticket.h"

#include "ui_order_widget.h"

namespace Ui {
class OrderWidget;
}

class OrderWidget : public QWidget {
    Q_OBJECT

public:
    explicit OrderWidget(QWidget* parent = nullptr);
    explicit OrderWidget(const Ticket& ticket, QWidget* parent = nullptr);
    ~OrderWidget();

    void updateOrderData(const Ticket& ticket);
    void updateOrderData(int orderNumber, 
                         const QDate& orderDate,
                         const QTime& departureTime,
                         const QString& departurePlace,
                         const QTime& arrivalTime,
                         const QString& arrivalPlace);

    Ticket getTicket() const;
    int getOrderNumber() const;
    QDate getOrderDate() const;
    QTime getDepartureTime() const;
    QString getDeparturePlace() const;
    QTime getArrivalTime() const;
    QString getArrivalPlace() const;

signals:
    void downloadTicketClicked(const Ticket& ticket);
    void downloadCheckClicked(const Ticket& ticket);
    void widgetClicked(const Ticket& ticket);

private slots:
    void on_downloadTicket_clicked();
    void on_unfilledPushButton_downloadCheck_clicked();

private:
    void setupUI();
    void updateUIFromTicket();

private:
    Ui::OrderWidget* ui;
    Ticket currentTicket;
};