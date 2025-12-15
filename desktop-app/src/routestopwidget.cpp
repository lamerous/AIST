#include "routestopwidget.h"
#include "ui_route_stop_widget.h"

#include <QLabel>
#include <QDebug>

RouteStopWidget::RouteStopWidget(
    const QString& stopTime, 
    const QString& stopDate, 
    const QString& stopName, 
    const QString& stopAddress, 
    QWidget* parent
) : QWidget(parent)
    , ui(new Ui::RouteStopWidget)
{
    ui->setupUi(this);
    setupUI();
    updateStopData(stopTime, stopDate, stopName, stopAddress);
}

RouteStopWidget::~RouteStopWidget() {
    delete ui;
}

void RouteStopWidget::setupUI() {
    this->setFixedHeight(53);
}

void RouteStopWidget::updateStopData(
    const QString& stopTime, 
    const QString& stopDate, 
    const QString& stopName, 
    const QString& stopAddress
) {
    ui->departureTime->setText(stopTime);
    ui->departureDate->setText(stopDate);
    ui->departurePlace->setText(stopName);
    ui->address->setText(stopAddress);
}