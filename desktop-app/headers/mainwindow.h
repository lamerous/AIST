#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>

#include "schedule.h"
#include "routewidget.h"
#include "stopwidget.h"
#include "pathwidget.h"
#include "orderwidget.h"
#include "apiclient.h"
#include "user.h"

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
        LOGIN_PAGE = 1,
        REGISTER_PAGE = 2,
        PROFILE_PAGE = 3,
        SCHEDULE_PAGE = 4,
        ORDERS1_PAGE = 5,
        ORDERS2_PAGE = 6,
        EMPLOYEES1_PAGE = 7,
        EMPLOYEES2_PAGE = 8,
        CASHIER_PAGE = 9,
        ADD_ROUTE_PAGE = 10,
        ADD_PATH_PAGE = 11,
        ADD_STOP_PAGE = 12,
        EDIT_ROUTE_PAGE = 13,
        EDIT_PATH_PAGE = 14
    };

signals:
    void stopsInPathsUpdated();

private slots:
    void on_schedule1_menuButton_clicked();
    void swapPlaces_btn_clicked();

    void on_findStop_textChanged();
    void on_findPath_textChanged();

    void on_addStop_btn_clicked();
    void on_addPath_btn_clicked();
    void on_addRoute_btn_clicked();

    void on_login_btn_clicked();
    void on_register_btn_clicked();

    void on_addStopToPath_btn_clicked();
    void on_unfilledPushButton_editPath_apply_clicked();
    void on_unfilledPushButton_editPath_back_clicked();

    void on_logout_btn_clicked();

    void on_downloadTicket_clicked(const Ticket &ticket);
    void on_downloadCheck_clicked(const Ticket &ticket);

    void handleStops(const QList<BusStop> &stops);
    void handleStopCreate(const BusStop &stop);
    void handleStopDelete(int stopid);
    void handleStopError(const QString &error);

    void handlePaths(const QList<Path> &paths);
    void handlePath(const Path &path);
    void handlePathCreate(const Path &path);
    void handlePathUpdate(const Path &path);
    void handlePathDelete(int stopId);
    void handlePathError(const QString &error);
    void handleStopsInPathUpdate();

    void handleRoutes(const QList <Route> &routes);
    void handleRouteCreate(const Route &route);
    void handleRouteDelete(int routeId);
    void handleRouteError(const QString &error);

    void findRoutes();
    void handleRouteSell(const Route &route);
    
    void handleTickets(const QList <Ticket> &tickets);
    void handleTicketCreate(const Ticket &ticket);
    void handleTicketError(const QString &error);

    void handleRegister(const QString &message);
    void handleLogin(const User &user, const QString &cookie);
    void handleUser(const User &user);
    void handleAuthError(const QString &error);
    void handleRegisterError(const QString &error);
    void handleLoginError(const QString &error);
    void handleGetUserError(const QString &error);

    void onStopDelete(const BusStop& stop);
    void onPathDelete(const Path& path);
    void onPathEdit(Path& path);

    void updateServerData();

    void updateUserData(const User &user);
    void clearLayout(QLayout *layout);
    QString roleMatcher(const QString &role);
    void clearRouteFields();
    void switchPage(int pageIndex);
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;
    QVector<RouteWidget*> routeWidgets;
    Schedule schedule;
    Path currentEditingPath;
    User currentUser;

    ApiClient* client;

    QButtonGroup *dispatcherMenu_buttons_routes;
    QButtonGroup *dispatcherMenu_buttons_paths;
    QButtonGroup *dispatcherMenu_buttons_stops;
    QButtonGroup *swapPlaces_buttons;
    QButtonGroup *loginMenu_buttons;
    QButtonGroup *registerMenu_buttons;
    QButtonGroup *findRoute_buttons;
};

#endif