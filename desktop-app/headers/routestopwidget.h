#pragma once

#include <QWidget>

namespace Ui {
class RouteStopWidget;
}

class RouteStopWidget : public QWidget {
    Q_OBJECT

public:
    explicit RouteStopWidget(
        const QString& stopTime, 
        const QString& stopDate, 
        const QString& stopName, 
        const QString& stopAddress, 
        QWidget* parent = nullptr
    );
    ~RouteStopWidget();

    void updateStopData(
        const QString& stopTime, 
        const QString& stopDate, 
        const QString& stopName, 
        const QString& stopAddress
    );

private:
    void setupUI();

private:
    Ui::RouteStopWidget* ui;
};