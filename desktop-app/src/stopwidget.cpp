#include "stopwidget.h"
#include "ui_stop_widget.h"

#include <QLabel>
#include <QDebug>

StopWidget::StopWidget(const BusStop& busstop, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::StopWidget)
    , currentStop(busstop)
{
    ui->setupUi(this);
    setupUI();
    updateStopData(busstop);
}

StopWidget::~StopWidget() {
    delete ui;
}

void StopWidget::setupUI() {

}

void StopWidget::updateStopData(const BusStop& busstop) {
    currentStop = busstop;
    
    ui->stop->setText(busstop.getStopName() + " (" + busstop.getStopAddress() + ")");
}

BusStop StopWidget::getStop() const {
    return currentStop;
}

void StopWidget::on_delButton_clicked() {
    emit delButtonClicked(currentStop);
    qDebug() << "Удалена остановка" << currentStop.getStopName() << " ID: " << currentStop.getStopId();
}