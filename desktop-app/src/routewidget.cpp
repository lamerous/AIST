#include "routewidget.h"
#include "ui_route_widget.h"
#include "routestopwidget.h"
#include "busstop.h"

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
    ui->roundedWidget_routeDetail->hide();

    connect(ui->details_btn, &QPushButton::toggled, [this](bool checked) {
        QPixmap pixmap;
        if (checked) {
            pixmap.load(":/icons/icons/arrow-down.png");
            ui->detail_widget->setStyleSheet(
                "border-bottom-left-radius: 24px;"
                "border-bottom-right-radius: 24px;"
                "background-color: #f2f2f2;"
            );
        }
        else {
            pixmap.load(":/icons/icons/arrow-up.png");
            ui->detail_widget->setStyleSheet(
                "border-bottom-left-radius: 0px;"
                "border-bottom-right-radius: 0px;"
                "background-color: #f2f2f2;"
            );
        }
        ui->routeDetail_arrow->setPixmap(pixmap);
    });
}

void RouteWidget::updateRouteData(const Route& route) {
    currentRoute = route;
    
    ui->departureTime->setText(route.getDepartureTime().toString("hh:mm"));
    ui->arivingTime->setText(route.getDestinationTime().toString("hh:mm"));
    
    ui->departurePlace->setText(route.getDeparturePlace().getStopName());
    ui->destinationPlace->setText(route.getDestinationPlace().getStopName());
    
    calculateTravelTime();
    
    ui->routePrice_label->setText(QString::number(route.getPrice()) + " руб.");
    ui->placeCount_label->setText(QString::number(route.getSeats()) + " мест осталось");

    QLocale locale(QLocale::Russian); 

    QDate departureDate = route.getDepartureDate();
    QTime departureTime = route.getDepartureTime();
    QDateTime departureDateTime(departureDate, departureTime);

    int travelSeconds = route.getDepartureTime().secsTo(route.getDestinationTime());
    QDateTime destinationDateTime = departureDateTime.addSecs(travelSeconds);
    QString departureStr = locale.toString(departureDate, "dd MMM, ddd");
    QString destinationStr = locale.toString(destinationDateTime, "dd MMM, ddd");

    ui->departureDate->setText(departureStr);
    ui->destinationDate->setText(destinationStr);

    QVector <BusStop> stops = route.getPath().getStops();
    int i = 0;
    for (const BusStop &stop : stops) {
        QString stopTime = "", stopDate = "";

        if (i == 0) {
            stopTime = departureTime.toString("hh:mm");
            stopDate = departureStr;
        }
        else if (i == stops.size()-1) {
            stopTime = destinationDateTime.toString("hh:mm");
            stopDate = departureStr;
        }

        RouteStopWidget *stopWidget = new RouteStopWidget(
            stopTime, 
            stopDate, 
            stop.getStopName(),
            stop.getStopAddress()
        );
        ui->stopsLayout->addWidget(stopWidget);
        i++;
    }

    ui->routeNumber->setText(QString::number(route.getId()));
    ui->pathNumber->setText(route.getPathNumber());
    ui->route->setText(route.getPath().getFirstStop().getStopName() + "—" + route.getPath().getLastStop().getStopName());
    ui->routeDate->setText(departureDate.toString("dd.MM.yyyy"));
    ui->routeTime->setText(departureTime.toString("hh:mm"));
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
    qDebug() << "Куплен билет на маршрут:" << currentRoute.getPathNumber();
}