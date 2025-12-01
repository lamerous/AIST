#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>

#include "schedule.h"
#include "routewidget.h"
#include "stopwidget.h"
#include "pathwidget.h"
#include "apiclient.h"
#include "busstopapiclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum Page {
        HOME_PAGE = 0,
        PROFILE_PAGE = 1,
        SCHEDULE_PAGE = 2,
        ORDERS1_PAGE = 3,
        ORDERS2_PAGE = 4,
        EMPLOYEES1_PAGE = 5,
        EMPLOYEES2_PAGE = 6,
        CASHIER_PAGE = 7,
        ADD_ROUTE_PAGE = 8,
        ADD_PATH_PAGE = 9,
        ADD_STOP_PAGE = 10,
        DISPATCHER_EDIT_PAGE = 11
    };

private slots:
    void clearRouteFields();
    void switchPage(int pageIndex);

    void on_schedule1_menuButton_clicked();

    void on_findRoute2_btn_clicked();
    void on_swapPlaces_btn_clicked();

    void on_addRoute_btn_clicked();
    void on_addPath_btn_clicked();
    void on_addStop_btn_clicked();

    void keyPressEvent(QKeyEvent *event);

    void onRoutesReceived(const QList<Route> &routes);
    void onRouteAdded(bool success);
    void onErrorOccurred(const QString &errorMessage);

    void onBusStopsReceived(const QList<BusStop> &busStops);
    void onBusStopAdded(bool success);
    void onBusStopDeleted(bool success);
    void onBusStopErrorOccurred(const QString &errorMessage);

    void onStopDelete(const BusStop& busStop);

private:
    Ui::MainWindow *ui;
    QVector<RouteWidget*> routeWidgets;
    Schedule schedule;
    ApiClient* apiClient;
    BusStopApiClient* busStopApiClient;

    QButtonGroup *dispatcherMenu_buttons_routes;
    QButtonGroup *dispatcherMenu_buttons_paths;
    QButtonGroup *dispatcherMenu_buttons_stops;
    QButtonGroup *dispatcherMenu_buttons_dispEdit;
    QButtonGroup *swapPlaces_buttons;
};

#endif