#pragma once

#include <QWidget>
#include "route.h"

namespace Ui {
class RouteWidget;
}

class RouteWidget : public QWidget {
    Q_OBJECT

public:
    explicit RouteWidget(const Route& route, QWidget* parent = nullptr);
    ~RouteWidget();

    void updateRouteData(const Route& route);
    
    Route getRoute() const;

signals:
    void buyButtonClicked(const Route& route);
    void widgetClicked(const Route& route);

private slots:
    void on_buyRoute_btn_clicked();

private:
    void setupUI();
    void calculateTravelTime();
    QString formatTravelTime() const;

private:
    Ui::RouteWidget* ui;
    Route currentRoute;
};