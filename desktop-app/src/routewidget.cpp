#include "routewidget.h"
#include "ui_route_widget.h"

#include <QLabel>
#include <QDebug>

RouteWidget::RouteWidget(const Route& route, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::RouteWidget)
    , currentRoute(route)
{
    ui->setupUi(this);
    setupUI();
    updateRouteData(route);
}

RouteWidget::~RouteWidget() {
    delete ui;
}

void RouteWidget::setupUI() {
    this->setObjectName("roundedWidget");
}

void RouteWidget::updateRouteData(const Route& route) {
    currentRoute = route;
    
    ui->departureTime->setText(route.getDepartureTime().toString("hh:mm"));
    ui->arivingTime->setText(route.getDestinationTime().toString("hh:mm"));
    
    ui->departurePlace->setText(route.getDeparturePlace());
    ui->destinationPlace->setText(route.getDestinationPlace());
    
    calculateTravelTime();
    
    ui->routePrice_label->setText(QString::number(route.getPrice()) + " руб.");
    ui->placeCount_label->setText(QString::number(route.getSeats()) + " мест осталось");
}

Route RouteWidget::getRoute() const {
    return currentRoute;
}

void RouteWidget::calculateTravelTime() {
    if (!currentRoute.getDepartureTime().isValid() || 
        !currentRoute.getDestinationTime().isValid()) {
        ui->travelTime->setText("Время не указано");
        return;
    }
    
    int departureSecs = currentRoute.getDepartureTime().msecsSinceStartOfDay() / 1000;
    int destinationSecs = currentRoute.getDestinationTime().msecsSinceStartOfDay() / 1000;
    
    int travelSeconds = destinationSecs - departureSecs;
    if (travelSeconds < 0) {
        travelSeconds += 24 * 3600;
    }
    
    int hours = travelSeconds / 3600;
    int minutes = (travelSeconds % 3600) / 60;
    
    ui->travelTime->setText(formatTravelTime());
}

QString RouteWidget::formatTravelTime() const {
    if (!currentRoute.getDepartureTime().isValid() || 
        !currentRoute.getDestinationTime().isValid()) {
        return "Время не указано";
    }
    
    int departureSecs = currentRoute.getDepartureTime().msecsSinceStartOfDay() / 1000;
    int destinationSecs = currentRoute.getDestinationTime().msecsSinceStartOfDay() / 1000;
    
    int travelSeconds = destinationSecs - departureSecs;
    if (travelSeconds < 0) {
        travelSeconds += 24 * 3600;
    }
    
    int hours = travelSeconds / 3600;
    int minutes = (travelSeconds % 3600) / 60;
    
    QStringList parts;
    if (hours > 0) {
        parts.append(QString("%1 ч.").arg(hours));
    }
    if (minutes > 0) {
        parts.append(QString("%1 мин.").arg(minutes));
    }
    
    return parts.join(" ") + " в пути";
}

void RouteWidget::on_buyRoute_btn_clicked() {
    emit buyButtonClicked(currentRoute);
    qDebug() << "Куплен билет на маршрут:" << currentRoute.getRouteNumber();
}