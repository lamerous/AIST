#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>
#include <QButtonGroup>

#include "mainwindow.h"
#include "ui_main_window.h"

#include "route.h"
#include "path.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menu_layout_2->hide();

    ui->stackedWidget->setCurrentIndex(0);
    ui->route_label->setText("");

    dispatcherMenu_buttons_routes = new QButtonGroup(this);
    dispatcherMenu_buttons_paths = new QButtonGroup(this);
    dispatcherMenu_buttons_stops = new QButtonGroup(this);
    swapPlaces_buttons = new QButtonGroup(this);
    loginMenu_buttons = new QButtonGroup(this);
    registerMenu_buttons = new QButtonGroup(this);
    
    dispatcherMenu_buttons_routes->addButton(ui->addRoute_textBtn);
    dispatcherMenu_buttons_routes->addButton(ui->addRoute2_textBtn);
    dispatcherMenu_buttons_routes->addButton(ui->addRoute3_textBtn);
    dispatcherMenu_buttons_routes->addButton(ui->addRoute5_textBtn);

    dispatcherMenu_buttons_paths->addButton(ui->addPath_textBtn);
    dispatcherMenu_buttons_paths->addButton(ui->addPath2_textBtn);
    dispatcherMenu_buttons_paths->addButton(ui->addPath3_textBtn);
    dispatcherMenu_buttons_paths->addButton(ui->addPath5_textBtn);

    dispatcherMenu_buttons_stops->addButton(ui->addStop_textBtn);
    dispatcherMenu_buttons_stops->addButton(ui->addStop2_textBtn);
    dispatcherMenu_buttons_stops->addButton(ui->addStop3_textBtn);
    dispatcherMenu_buttons_stops->addButton(ui->addStop5_textBtn);

    swapPlaces_buttons->addButton(ui->swapPlaces_btn);
    swapPlaces_buttons->addButton(ui->swapPlaces2_btn);

    loginMenu_buttons->addButton(ui->login_textBtn);
    loginMenu_buttons->addButton(ui->login2_textBtn);

    registerMenu_buttons->addButton(ui->register_textBtn);
    registerMenu_buttons->addButton(ui->register2_textBtn);

    connect(ui->logo_textBtn, &QPushButton::pressed, [this]() {
        ui->stackedWidget->setCurrentIndex(HOME_PAGE);
    });
    connect(ui->profile1_menuButton, &QPushButton::toggled, [this](bool checked) {
        if (checked) ui->stackedWidget->setCurrentIndex(PROFILE_PAGE);
    });
    connect(ui->cabinet_textBtn, &QPushButton::pressed, [this]() {
        ui->stackedWidget->setCurrentIndex(PROFILE_PAGE);
    });
    connect(ui->schedule1_menuButton, &QPushButton::toggled, [this](bool checked) {
        if (checked) ui->stackedWidget->setCurrentIndex(SCHEDULE_PAGE);
    });
    connect(ui->cart1_menuButton, &QPushButton::toggled, [this](bool checked) {
        if (checked) ui->stackedWidget->setCurrentIndex(ORDERS1_PAGE);
    });
    connect(ui->employees1_menuButton, &QPushButton::toggled, [this](bool checked) {
        if (checked) ui->stackedWidget->setCurrentIndex(EMPLOYEES1_PAGE);
    });
    connect(ui->currentOrders2_textBtn, &QPushButton::pressed, [this]() {
        ui->stackedWidget->setCurrentIndex(ORDERS1_PAGE);
    });
    connect(ui->oldOrders_textBtn, &QPushButton::pressed, [this]() {
        ui->stackedWidget->setCurrentIndex(ORDERS2_PAGE);
    });
    connect(ui->cashier1_menuButton, &QPushButton::toggled, [this](bool checked) {
        if (checked) ui->stackedWidget->setCurrentIndex(CASHIER_PAGE);
    });
    connect(ui->currentOrders2_textBtn, &QPushButton::pressed, [this]() {
        ui->stackedWidget->setCurrentIndex(ORDERS1_PAGE);
    });
    connect(ui->oldOrders_textBtn, &QPushButton::pressed, [this]() {
        ui->stackedWidget->setCurrentIndex(ORDERS2_PAGE);
    });
    connect(ui->dispatcher1_menuButton, &QPushButton::toggled, [this](bool checked) {
        if (checked) ui->stackedWidget->setCurrentIndex(ADD_ROUTE_PAGE);
    });

    connect(dispatcherMenu_buttons_routes, &QButtonGroup::idClicked, [this] {
        ui->stackedWidget->setCurrentIndex(ADD_ROUTE_PAGE);
    });
    connect(dispatcherMenu_buttons_paths, &QButtonGroup::idClicked, [this] {
        ui->stackedWidget->setCurrentIndex(ADD_PATH_PAGE);
    });
    connect(dispatcherMenu_buttons_stops, &QButtonGroup::idClicked, [this] {
        ui->stackedWidget->setCurrentIndex(ADD_STOP_PAGE);
    });
    connect(loginMenu_buttons, &QButtonGroup::idClicked, [this] {
        ui->stackedWidget->setCurrentIndex(LOGIN_PAGE);
    });
    connect(registerMenu_buttons, &QButtonGroup::idClicked, [this] {
        ui->stackedWidget->setCurrentIndex(REGISTER_PAGE);
    });

    connect(swapPlaces_buttons, &QButtonGroup::idClicked, [this] {
        MainWindow::on_swapPlaces_btn_clicked();
    });

    // Если открыта страница с диспетчерским меню
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index){
        if (index == ADD_ROUTE_PAGE) {
            ui->addRoute_status->setText("");
        }
        if (index == ADD_PATH_PAGE) {
            ui->addPath_status->setText("");
        }
        if (index == ADD_STOP_PAGE) {
            ui->addStop_status->setText("");
        }
        if (index == EDIT_PATH_PAGE) {
            ui->editPath_status->setText("");
        }
        if (index == SCHEDULE_PAGE) {
            MainWindow::on_schedule1_menuButton_clicked();
        }
    });


    client = new ApiClient(this);

    // connect(client->routes(), &RouteApiClient::routesReceived, this, &MainWindow::handleRoutes);
    // connect(client->routes(), &RouteApiClient::routeReceived, this, &MainWindow::handleRoute);
    // connect(client->routes(), &RouteApiClient::routeCreated, this, &MainWindow::handleRouteCreate);
    // connect(client->routes(), &RouteApiClient::requestError, this, &MainWindow::handleRouteError);

    connect(client->stops(), &StopApiClient::stopsReceived, this, &MainWindow::handleStops);
    connect(client->stops(), &StopApiClient::stopCreated, this, &MainWindow::handleStopCreate);
    connect(client->stops(), &StopApiClient::stopDeleted, this, &MainWindow::handleStopDelete);
    connect(client->stops(), &StopApiClient::requestError, this, &MainWindow::handleStopError);

    connect(client->paths(), &PathApiClient::pathsReceived, this, &MainWindow::handlePaths);
    connect(client->paths(), &PathApiClient::pathReceived, this, &MainWindow::handlePath);
    connect(client->paths(), &PathApiClient::pathCreated, this, &MainWindow::handlePathCreate);
    connect(client->paths(), &PathApiClient::pathUpdated, this, &MainWindow::handlePathUpdate);
    connect(client->paths(), &PathApiClient::pathDeleted, this, &MainWindow::handlePathDelete);
    connect(client->paths(), &PathApiClient::requestError, this, &MainWindow::handlePathError);

    connect(client->routes(), &RouteApiClient::routesReceived, this, &MainWindow::handleRoutes);
    connect(client->routes(), &RouteApiClient::routeCreated, this, &MainWindow::handleRouteCreate);
    connect(client->routes(), &RouteApiClient::routeDeleted, this, &MainWindow::handleRouteDelete);
    connect(client->routes(), &RouteApiClient::requestError, this, &MainWindow::handleRouteError);

    connect(client->auths(), &AuthApiClient::userRegistred, this, &MainWindow::handleRegister);
    connect(client->auths(), &AuthApiClient::userLogedIn, this, &MainWindow::handleLogin);
    connect(client->auths(), &AuthApiClient::requestError, this, &MainWindow::handleAuthError);

    connect(this, &MainWindow::stopsInPathsUpdated, this, &MainWindow::handleStopsInPathUpdate);

    client->auths()->setBaseUrl("http://localhost:8002");

    client->stops()->getAllStops();
    client->paths()->getAllPaths();
    client->routes()->getAllRoutes();
}

MainWindow::~MainWindow() {
    delete ui;
}

// ================= STOPS ==================
void MainWindow::on_addStop_btn_clicked() {
    QString stopName = ui->addStop_name->text();
    QString stopAddress = ui->addStop_address->text();

    ui->addStop_status->setStyleSheet("color: black;");
    ui->addStop_status->setText("Отправка запроса");
    BusStop stop(stopName, stopAddress);
    client->stops()->createStop(stop);
}

void MainWindow::on_findStop_textChanged() {
    QString search = ui->findStop->text();

    client->stops()->getAllStops(0, 100, search);
}

void MainWindow::handleStopCreate(const BusStop &stop) {
    client->stops()->getAllStops();
    ui->addStop_status->setStyleSheet("color: green;");
    ui->addStop_status->setText("Остановка добавлена");
}

void MainWindow::handleStops(const QList<BusStop> &stops) {
    QLayoutItem* item;
    while ((item = ui->stopsListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    schedule.clear();
    
    for (const BusStop &stop : stops) {
        StopWidget *stopWidget = new StopWidget(stop);
        ui->stopsListLayout->insertWidget(0, stopWidget);
        connect(stopWidget, &StopWidget::delButtonClicked, this, &MainWindow::onStopDelete);
    }
}

void MainWindow::onStopDelete(const BusStop &stop) {
    client->stops()->deleteStop(stop.getStopId());
}

void MainWindow::handleStopDelete(int stopId) {
    client->stops()->getAllStops();
}

void MainWindow::handleStopError(const QString &error) {
    qDebug() << "Ошибка при обащении к /api/stops:" << error;
    ui->addStop_status->setStyleSheet("color: red;");
    ui->addStop_status->setText("Ошибка отправки запроса\n"+error);
}


// =============== PATH ============
void MainWindow::on_addPath_btn_clicked() {
    QString pathNumber = ui->addPath_pathNumber->text();

    ui->addStop_status->setStyleSheet("color: black;");
    ui->addStop_status->setText("Отправка запроса");

    Path path(pathNumber);

    client->paths()->createPath(path);
}

void MainWindow::on_findPath_textChanged() {
    QString search = ui->findPath->text();
    client->paths()->getAllPaths(0, 100, search);
}

void MainWindow::handlePaths(const QList<Path> &paths) {
    QLayoutItem* item;
    while ((item = ui->pathsListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    for (const Path &path : paths) {
        PathWidget *pathWidget = new PathWidget(path);
        ui->pathsListLayout->insertWidget(0, pathWidget);
        connect(pathWidget, &PathWidget::editButtonClicked, this, &MainWindow::onPathEdit);
        connect(pathWidget, &PathWidget::delButtonClicked, this, &MainWindow::onPathDelete);
    }
}

void MainWindow::handlePath(const Path &path) {

}

void MainWindow::handlePathCreate(const Path &path) {
    client->paths()->getAllPaths();
    ui->addStop_status->setStyleSheet("color: green;");
    ui->addStop_status->setText("Остановка добавлена");
}

void MainWindow::handlePathUpdate(const Path &path) {
    client->paths()->getAllPaths();
    ui->editPath_status->setStyleSheet("color: green;");
    ui->editPath_status->setText("Маршрут обновлен\n");
}

void MainWindow::handlePathDelete(int stopId) {
    client->paths()->getAllPaths();
}

void MainWindow::handlePathError(const QString &error) {
    qDebug() << "Ошибка при обащении к /api/paths:" << error;
    ui->addPath_status->setStyleSheet("color: red;");
    ui->addPath_status->setText("Ошибка отправки запроса\n"+error);
}

void MainWindow::onPathDelete(const Path &path) {
    client->paths()->deletePath(path.getPathId());
}


void MainWindow::onPathEdit(Path &path) {
    ui->stackedWidget->setCurrentIndex(EDIT_PATH_PAGE);
    
    currentEditingPath = path;
    
    ui->editPath_pathNumber->setText(currentEditingPath.getNumber());

    handleStopsInPathUpdate();
}

void MainWindow::handleStopsInPathUpdate() {
    QLayoutItem* item;
    while ((item = ui->editStopsListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (const BusStop &stop : currentEditingPath.getStops()) {
        StopWidget *stopWidget = new StopWidget(stop);
        ui->editStopsListLayout->insertWidget(0, stopWidget);
        connect(stopWidget, &StopWidget::delButtonClicked, 
            this, [this](const BusStop& busStop){
                currentEditingPath.removeStopById(busStop.getStopId());
                handleStopsInPathUpdate();
            });
    }
}

void MainWindow::on_addStopToPath_btn_clicked() {
    QString stopName = ui->editPath_stopName->text();
    currentEditingPath.addStop(BusStop(stopName, "testAddr"));
    handleStopsInPathUpdate();
}

void MainWindow::on_unfilledPushButton_editPath_apply_clicked() {
    client->paths()->updatePath(currentEditingPath.getPathId(), currentEditingPath);
    ui->addStop_status->setStyleSheet("color: green;");
    ui->addStop_status->setText("Маргрут обновлен");
}

void MainWindow::on_unfilledPushButton_editPath_back_clicked() {
    ui->stackedWidget->setCurrentIndex(ADD_PATH_PAGE);
}

// ========== ROUTES =============
void MainWindow::on_addRoute_btn_clicked(){
    QString number = ui->addRoute_number->text();
    QString platform = ui->addRoute_platform->text();
    
    QDate date = QDate::fromString(ui->addRoute_departureDate->text(), "dd.MM.yyyy");
    QTime depTime = QTime::fromString(ui->addRoute_departureTime->text(), "hh:mm");
    QTime destTime = QTime::fromString(ui->addRoute_destinationTime->text(), "hh:mm");

    int price = ui->addRoute_price->text().toInt();
    int seats = ui->addRoute_seats->text().toInt();
    
    if (number == "" || platform == "" || price == 0 || seats == 0) {
        ui->addRoute_status->setStyleSheet("color: red;");
        ui->addRoute_status->setText("Заполните все поля");

        qCritical() << "Ошибка добавления маршрута: Не все значения введены";
        return;
    }

    if (price < 0) {
        ui->addRoute_status->setStyleSheet("color: red;");
        ui->addRoute_status->setText("Цена должна быть больше 0");

        qCritical() << "Ошибка добавления маршрута: Цена должна быть больше 0";
        return;
    }

    if (seats < 0) {
        ui->addRoute_status->setStyleSheet("color: red;");
        ui->addRoute_status->setText("Кол-во мест должно быть больше 0");

        qCritical() << "Ошибка добавления маршрута: Кол-во мест должно быть больше 0";
        return;
    }

    if (!date.isValid()) {
        ui->addRoute_status->setStyleSheet("color: red;");
        ui->addRoute_status->setText("Неверная дата");

        qCritical() << "Ошибка добавления маршрута: Неверная дата";
        return;
    }
    if (!depTime.isValid()) {
        ui->addRoute_status->setStyleSheet("color: red;");
        ui->addRoute_status->setText("Неверное время отправления");

        qCritical() << "Ошибка добавления маршрута: Неверное время отправления";
        return;
    }
    if (!depTime.isValid() || !destTime.isValid()) {
        ui->addRoute_status->setStyleSheet("color: red;");
        ui->addRoute_status->setText("Неверное время прибытия");

        qCritical() << "Ошибка добавления маршрута: Неверное время прибытия";
        return;
    }
    
    Route route(number, platform, date, depTime, destTime, price, seats, Path());
    qDebug() << "Создан маршрут:" << route.toString();

    ui->addRoute_status->setStyleSheet("color: black;");
    ui->addRoute_status->setText("Отправка запроса");

    client->routes()->createRoute(route);
    qDebug() << "Маршрут добавлен в глобальное расписание";

    ui->addRoute_status->setStyleSheet("color: green;");
    ui->addRoute_status->setText("Маршрут добавлен в расписание");

    schedule.add(Route);
    client->routes()->getAllRoutes();
    clearRouteFields();
}

void MainWindow::clearRouteFields() {
    ui->addRoute_number->clear();
    ui->addRoute_platform->clear();
    ui->addRoute_price->clear();
    ui->addRoute_departureDate->clear();
    ui->addRoute_departureTime->clear();
    ui->addRoute_destinationTime->clear();
    ui->addRoute_seats->clear();
}

void MainWindow::handleRoutes(const QList <Route> &routes) {
    QLayoutItem* item;
    while ((item = ui->routesListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    while ((item = ui->routes_layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (const Route &route : schedule.getAllRoutes()) {
        RouteWidget *routeWidget = new RouteWidget(route);
        ui->routesListLayout->insertWidget(0, routeWidget);
        ui->routes_layout->insertWidget(0, routeWidget);
        connect(routesWidget, &RouteWidget::buyButtonClicked, this, &MainWindow::handleRouteSell);
    }

    int offers = routes.size();
    QString offer_string = QString::number(offers);

    if ((offers % 10) == 0 || (offers % 10) >= 5) offer_string += " предложений";
    else if ((offers % 10) == 1) offer_string += " предложение";
    else if ((offers % 10) >= 2 && (offers % 10) < 5) offer_string += " предложения"; 

    ui->offerCount_label->setText(offer_string);
    schedule.displayAll();
}

void MainWindow::onRouteSell(const Route &route) {
    QLayoutItem* item;
    while ((item = ui->ordersListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    
}

void MainWindow::handleRouteCreate(const Route &route) {
    client->routes()->getAllRoutes();
    ui->addStop_status->setStyleSheet("color: green;");
    ui->addStop_status->setText("Рейс добавлен");
}

void MainWindow::handleRouteDelete(int routeId) {

}

void MainWindow::handleRouteError(const QString &error) {
    qDebug() << "Ошибка при обащении к /api/routes:" << error;
    ui->addRoute_status->setStyleSheet("color: red;");
    ui->addRoute_status->setText("Ошибка отправки запроса\n"+error);
}

void MainWindow::on_findRoute2_btn_clicked() {
    ui->route_label->setText("");
    QString tripDeparture2 = ui->tripDeparture2->text();
    QString tripDestination2 = ui->tripDestination2->text();

    if (ui->tripDeparture2->text() != "" && ui->tripDestination2->text() != "") {
        ui->route_label->setText(tripDeparture2 + "—" + tripDestination2);
    }
}

// ================= AUTH ===================
void MainWindow::on_login_btn_clicked() {
    QString username = ui->login_username->text();
    QString password = ui->login_password->text();

    client->auths()->login(username, password);
}

void MainWindow::on_register_btn_clicked() {
    QString username = ui->register_username->text();
    QString last_name = ui->register_lastname->text();
    QString first_name = ui->register_firstname->text();
    QString middle_name = ui->register_middlename->text();
    QString phone = ui->register_phone->text();
    QString email = ui->register_email->text();
    QDate birth_date = QDate::fromString(ui->register_birthdate->text(), "dd.MM.yyyy");
    QString password = ui->register_password->text();

    client->auths()->regist(username, last_name, first_name, middle_name, phone,
                            email, birth_date, password);
}

void MainWindow::handleRegister(const QString &message) {
    qDebug() << "USER REGISTREED :DD" << message;
}

void MainWindow::handleLogin(const QString &cookie) {
    qDebug() << "USER LOGGED IN :DD " << cookie;
    client->setAccessToken(cookie);

    QFile file;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out("cookie");
    out << cookie;
    
    file.close();
}

void MainWindow::handleAuthError(const QString &error) {
    qDebug() << error;
}


void MainWindow::on_schedule1_menuButton_clicked() {
    
}


void MainWindow::on_swapPlaces_btn_clicked() {
    QString temp = ui->tripDeparture2->text();
    ui->tripDeparture2->setText(ui->tripDestination2->text());
    ui->tripDestination2->setText(temp);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();

    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        int pageIndex = key - Qt::Key_0;
        if (pageIndex < ui->stackedWidget->count()) {
            ui->stackedWidget->setCurrentIndex(pageIndex);
        }
    }
}

void MainWindow::switchPage(int pageIndex) {
    if (pageIndex >= 0 && pageIndex < ui->stackedWidget->count()) {
        ui->stackedWidget->setCurrentIndex(pageIndex);
    }
}