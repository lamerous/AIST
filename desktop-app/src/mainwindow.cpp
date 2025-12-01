#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>
#include <QButtonGroup>

#include "mainwindow.h"
#include "ui_main_window.h"

#include "route.h"

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
    dispatcherMenu_buttons_dispEdit = new QButtonGroup(this);
    swapPlaces_buttons = new QButtonGroup(this);
    
    dispatcherMenu_buttons_routes->addButton(ui->addRoute_textBtn);
    dispatcherMenu_buttons_routes->addButton(ui->addRoute2_textBtn);
    dispatcherMenu_buttons_routes->addButton(ui->addRoute3_textBtn);
    // dispatcherMenu_buttons_routes->addButton(ui->addRoute4_textBtn);

    dispatcherMenu_buttons_paths->addButton(ui->addPath_textBtn);
    dispatcherMenu_buttons_paths->addButton(ui->addPath2_textBtn);
    dispatcherMenu_buttons_paths->addButton(ui->addPath3_textBtn);
    // dispatcherMenu_buttons_paths->addButton(ui->addPath4_textBtn);

    dispatcherMenu_buttons_stops->addButton(ui->addStop_textBtn);
    dispatcherMenu_buttons_stops->addButton(ui->addStop2_textBtn);
    dispatcherMenu_buttons_stops->addButton(ui->addStop3_textBtn);
    // dispatcherMenu_buttons_routes->addButton(ui->addStop4_textBtn);

    swapPlaces_buttons->addButton(ui->swapPlaces_btn);
    swapPlaces_buttons->addButton(ui->swapPlaces2_btn);


    connect(ui->logo_textBtn, &QPushButton::pressed, [this]() {
        ui->stackedWidget->setCurrentIndex(HOME_PAGE);
    });
    connect(ui->profile1_menuButton, &QPushButton::toggled, [this](bool checked) {
        if (checked) ui->stackedWidget->setCurrentIndex(PROFILE_PAGE);
    });
    connect(ui->login_textBtn, &QPushButton::pressed, [this]() {
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
    connect(dispatcherMenu_buttons_dispEdit, &QButtonGroup::idClicked, [this] {
        ui->stackedWidget->setCurrentIndex(DISPATCHER_EDIT_PAGE);
    });

    connect(swapPlaces_buttons, &QButtonGroup::idClicked, [this] {
        MainWindow::on_swapPlaces_btn_clicked();
    });

    // Если открыта страница с диспетчерским меню
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index){
        if (index == ADD_ROUTE_PAGE) {
            ui->addRoute_status->setText("");
        }
        if (index == ADD_STOP_PAGE) {
            ui->addStop_status->setText("");
        }
        if (index == SCHEDULE_PAGE) {
            MainWindow::on_schedule1_menuButton_clicked();
        }
    });


    apiClient = new ApiClient(this);
    busStopApiClient = new BusStopApiClient(this);

    connect(apiClient, &ApiClient::routesReceived, this, &MainWindow::onRoutesReceived);
    connect(apiClient, &ApiClient::routeAdded, this, &MainWindow::onRouteAdded);
    connect(apiClient, &ApiClient::errorOccurred, this, &MainWindow::onErrorOccurred);

    connect(busStopApiClient, &BusStopApiClient::busStopsReceived, this, &MainWindow::onBusStopsReceived);
    connect(busStopApiClient, &BusStopApiClient::busStopAdded, this, &MainWindow::onBusStopAdded);
    connect(busStopApiClient, &BusStopApiClient::busStopDeleted, this, &MainWindow::onBusStopDeleted);
    connect(busStopApiClient, &BusStopApiClient::errorOccurred, this, &MainWindow::onBusStopErrorOccurred);

    apiClient->getRoutes();
    busStopApiClient->getBusStops();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_addRoute_btn_clicked(){
    QString number = ui->addRoute_number->text();
    QString platform = ui->addRoute_platform->text();
    QString departure = ui->addRoute_departurePlace->text();
    QString destination = ui->addRoute_destinationPlace->text();
    
    QDate date = QDate::fromString(ui->addRoute_departureDate->text(), "dd.MM.yyyy");
    QTime depTime = QTime::fromString(ui->addRoute_departureTime->text(), "hh:mm");
    QTime destTime = QTime::fromString(ui->addRoute_destinationTime->text(), "hh:mm");

    int price = ui->addRoute_price->text().toInt();
    int seats = ui->addRoute_seats->text().toInt();
    
    if (number == "" || platform == "" || departure == "" || destination == "" || price == 0 || seats == 0) {
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
    
    Route route(number, platform, departure, destination, date, depTime, destTime, price, seats);
    qDebug() << "Создан маршрут:" << route.toString();

    ui->addRoute_status->setStyleSheet("color: black;");
    ui->addRoute_status->setText("Отправка запроса");
    apiClient->addRoute(route);
    qDebug() << "Маршрут добавлен в глобальное расписание";

    ui->addRoute_status->setStyleSheet("color: green;");
    ui->addRoute_status->setText("Маршрут добавлен в расписание");

    clearRouteFields();
}

void MainWindow::on_schedule1_menuButton_clicked() {
    apiClient->getRoutes();
    int offers = schedule.count();
    QString offer_string = QString::number(offers);

    if ((offers % 10) == 0 || (offers % 10) >= 5) offer_string += " предложений";
    else if ((offers % 10) == 1) offer_string += " предложение";
    else if ((offers % 10) >= 2 && (offers % 10) < 5) offer_string += " предложения"; 

    ui->offerCount_label->setText(offer_string);
    schedule.displayAll();
}

void MainWindow::onRoutesReceived(const QList<Route> &routes) {
    QLayoutItem* item;
    while ((item = ui->routes_layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    schedule.clear();
    
    for (const Route &route : routes) {
        RouteWidget *routeWidget = new RouteWidget(route);
        ui->routes_layout->addWidget(routeWidget);
        schedule.addRoute(route);
        
        // connect(routeWidget, &RouteWidget::buyButtonClicked,
        //         this, &MainWindow::onBuyButtonClicked);
    }
}

void MainWindow::onRouteAdded(bool success) {
    if (success) {
        qDebug() << "Маршрут успешно добавлен";
        apiClient->getRoutes();
    } else {
        qDebug() << "Ошибка при добавлении маршрута";
    }
}

void MainWindow::onErrorOccurred(const QString &errorMessage) {
    qDebug() << "Ошибка route API:" << errorMessage;
    QMessageBox::warning(this, "Ошибка route api", errorMessage);
}

void MainWindow::on_addStop_btn_clicked() {
    QString stopName = ui->addStop_name->text();
    QString stopAddress = ui->addStop_address->text();

    BusStop busStop(stopName, stopAddress);

    ui->addStop_status->setStyleSheet("color: black;");
    ui->addStop_status->setText("Отправка запроса");
    busStopApiClient->addBusStop(busStop);
}

void MainWindow::onBusStopsReceived(const QList<BusStop> &busStops) {
    QLayoutItem* item;
    while ((item = ui->stopsListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    schedule.clear();
    
    for (const BusStop &stop : busStops) {
        StopWidget *stopWidget = new StopWidget(stop);
        ui->stopsListLayout->addWidget(stopWidget);
        connect(stopWidget, &StopWidget::delButtonClicked, this, &MainWindow::onStopDelete);
        // schedule.addRoute(route);   CHANGE TO path.addStop(stop);
    }
}

void MainWindow::onBusStopAdded(bool success) {
    busStopApiClient->getBusStops();
    ui->addStop_status->setStyleSheet("color: green;");
    ui->addStop_status->setText("Информация об остановке добавлена");
}

void MainWindow::onBusStopDeleted(bool success) {
    busStopApiClient->getBusStops();
}

void MainWindow::onBusStopErrorOccurred(const QString &errorMessage) {
    qDebug() << "Ошибка bus_stop API:" << errorMessage;
    QMessageBox::warning(this, "Ошибка bus_stop_api", errorMessage);
}

void MainWindow::onStopDelete(const BusStop& busStop) {
    int busId = busStop.getStopId();

    qDebug() << "ID: " << busId << " Name: " << busStop.getStopName();

    busStopApiClient->deleteBusStop(busId);

    ui->addStop_status->setStyleSheet("color: black;");
    ui->addStop_status->setText("Остановка " + busStop.getStopName() + " удалена");
}

void MainWindow::on_findRoute2_btn_clicked() {
    ui->route_label->setText("");
    QString tripDeparture2 = ui->tripDeparture2->text();
    QString tripDestination2 = ui->tripDestination2->text();

    if (ui->tripDeparture2->text() != "" && ui->tripDestination2->text() != "") {
        ui->route_label->setText(tripDeparture2 + "—" + tripDestination2);
    }
}

void MainWindow::on_swapPlaces_btn_clicked() {
    QString temp = ui->tripDeparture2->text();
    ui->tripDeparture2->setText(ui->tripDestination2->text());
    ui->tripDestination2->setText(temp);
}

void MainWindow::clearRouteFields() {
    ui->addRoute_number->clear();
    ui->addRoute_platform->clear();
    ui->addRoute_departurePlace->clear();
    ui->addRoute_destinationPlace->clear();
    ui->addRoute_price->clear();
    ui->addRoute_departureDate->clear();
    ui->addRoute_departureTime->clear();
    ui->addRoute_destinationTime->clear();
    ui->addRoute_seats->clear();
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