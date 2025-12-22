#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>
#include <QButtonGroup>
#include <QFileDialog>

#include "jwtdecoder.h"

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

    ui->dispatcher1_menuButton->hide();
    ui->dispatcher2_menuButton->hide();
    ui->employees1_menuButton->hide();
    ui->employees2_menuButton->hide();
    ui->analytics1_menuButton->hide();
    ui->analytics2_menuButton->hide();

    ui->stackedWidget->setCurrentIndex(0);
    ui->route_label->setText("");

    dispatcherMenu_buttons_routes = new QButtonGroup(this);
    dispatcherMenu_buttons_paths = new QButtonGroup(this);
    dispatcherMenu_buttons_stops = new QButtonGroup(this);
    swapPlaces_buttons = new QButtonGroup(this);
    loginMenu_buttons = new QButtonGroup(this);
    registerMenu_buttons = new QButtonGroup(this);
    findRoute_buttons = new QButtonGroup(this);

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

    findRoute_buttons->addButton(ui->findRoute_btn);
    findRoute_buttons->addButton(ui->findRoute2_btn);

    connect(ui->logo_textBtn, &QPushButton::pressed, [this]() {
        ui->stackedWidget->setCurrentIndex(HOME_PAGE);
        ui->tripDeparture2->setText("");
        ui->tripDestination2->setText("");
    });
    connect(ui->profile1_menuButton, &QPushButton::toggled, [this](bool checked) {
        if (checked){
            if (currentUser.isAuthorized()){
                ui->stackedWidget->setCurrentIndex(PROFILE_PAGE);
            }
            else {
                ui->stackedWidget->setCurrentIndex(LOGIN_PAGE);
            }
        }
    });
    connect(ui->cabinet_textBtn, &QPushButton::pressed, [this]() {
        if (currentUser.isAuthorized()){
            ui->stackedWidget->setCurrentIndex(PROFILE_PAGE);
        }
        else {
            ui->stackedWidget->setCurrentIndex(LOGIN_PAGE);
        }
    });
    connect(ui->schedule1_menuButton, &QPushButton::toggled, [this](bool checked) {
        if (checked) ui->stackedWidget->setCurrentIndex(SCHEDULE_PAGE);
        ui->tripDeparture->setText("");
        ui->tripDestination->setText("");
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
    // connect(ui->cashier1_menuButton, &QPushButton::toggled, [this](bool checked) {
    //     if (checked) ui->stackedWidget->setCurrentIndex(CASHIER_PAGE);
    // });
    connect(ui->currentOrders2_textBtn, &QPushButton::pressed, [this]() {
        ui->stackedWidget->setCurrentIndex(ORDERS1_PAGE);
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

    connect(swapPlaces_buttons, &QButtonGroup::idClicked, this, &MainWindow::swapPlaces_btn_clicked);

    connect(findRoute_buttons, &QButtonGroup::idClicked, this, &MainWindow::findRoutes);

    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index){
        if (index == LOGIN_PAGE) {
            ui->login_status->setText("");
        }
        else if (index == REGISTER_PAGE) {
            ui->register_status->setText("");
        }
        else if (index == SCHEDULE_PAGE) {
            ui->route_status->setText("");
        }
        else if (index == ADD_ROUTE_PAGE) {
            ui->addRoute_status->setText("");
        }
        else if (index == ADD_PATH_PAGE) {
            ui->addPath_status->setText("");
        }
        else if (index == ADD_STOP_PAGE) {
            ui->addStop_status->setText("");
        }
        else if (index == EDIT_PATH_PAGE) {
            ui->editPath_status->setText("");
        }
        else if (index == SCHEDULE_PAGE) {
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

    connect(client->tickets(), &TicketApiClient::ticketsReceived, this, &MainWindow::handleTickets);
    connect(client->tickets(), &TicketApiClient::ticketCreated, this, &MainWindow::handleTicketCreate);
    connect(client->tickets(), &TicketApiClient::requestError, this, &MainWindow::handleTicketError);

    connect(client->auths(), &AuthApiClient::userRegistred, this, &MainWindow::handleRegister);
    connect(client->auths(), &AuthApiClient::userLogedIn, this, &MainWindow::handleLogin);
    connect(client->auths(), &AuthApiClient::getUser, this, &MainWindow::handleUser);
    connect(client->auths(), &AuthApiClient::requestError, this, &MainWindow::handleAuthError);
    connect(client->auths(), &AuthApiClient::registerError, this, &MainWindow::handleRegisterError);
    connect(client->auths(), &AuthApiClient::loginError, this, &MainWindow::handleLoginError);
    connect(client->auths(), &AuthApiClient::getUserError, this, &MainWindow::handleGetUserError);

    connect(this, &MainWindow::stopsInPathsUpdated, this, &MainWindow::handleStopsInPathUpdate);

    client->auths()->setBaseUrl("http://localhost:8002");

    updateServerData();

    currentUser.setRole("passenger");
    currentUser.checkCookieFile();
    if (currentUser.isAuthorized()) {
        qDebug() << "Устанавливаем куки";
        client->setAccessToken(currentUser.getCookie());
        client->auths()->get_user();
    }
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
    clearLayout(ui->stopsListLayout);
    
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
    clearLayout(ui->pathsListLayout);
    
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
    clearLayout(ui->editStopsListLayout);

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

    schedule.addRoute(route);
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
    clearLayout(ui->routesListLayout);
    clearLayout(ui->routes_layout);

    for (const Route &route : routes) {
        RouteWidget *routeWidget = new RouteWidget(route);
        ui->routesListLayout->insertWidget(0, routeWidget);
        ui->routes_layout->insertWidget(0, routeWidget);
        connect(routeWidget, &RouteWidget::buyButtonClicked, this, &MainWindow::handleRouteSell);
    }

    int offers = routes.size();
    QString offer_string = QString::number(offers);

    if ((offers % 10) == 0 || (offers % 10) >= 5) offer_string += " предложений";
    else if ((offers % 10) == 1) offer_string += " предложение";
    else if ((offers % 10) >= 2 && (offers % 10) < 5) offer_string += " предложения"; 

    ui->offerCount_label->setText(offer_string);
    schedule.displayAll();
}

void MainWindow::handleRouteSell(const Route &route) {
    if (currentUser.isAuthorized()) {
        client->tickets()->createTicket(route.getId());
    }
    else {
        ui->route_status->setStyleSheet("color: red;");
        ui->route_status->setText("Сперва войдите в аккаунт");
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

void MainWindow::findRoutes() {
    ui->route_label->setText("");
    ui->route_status->setText("");

    QString tripDeparture;
    QString tripDestination;

    if (ui->tripDeparture->text() != "" && ui->tripDestination->text() != "") {
        tripDeparture = ui->tripDeparture->text();
        tripDestination = ui->tripDestination->text();
    }

    if (ui->tripDeparture2->text() != "" && ui->tripDestination2->text() != "") {
        tripDeparture = ui->tripDeparture2->text();
        tripDestination = ui->tripDestination2->text();
    }

    ui->route_label->setText(tripDeparture + "—" + tripDestination);
    ui->tripDeparture2->setText(tripDeparture);
    ui->tripDestination2->setText(tripDestination);
    client->routes()->getAllRoutes(0, 100, "", tripDeparture, tripDestination);

    ui->stackedWidget->setCurrentIndex(SCHEDULE_PAGE);
}

// ================= TICKETS ===================
void MainWindow::handleTickets(const QList <Ticket> &tickets) {
    clearLayout(ui->ordersListLayout);

    for (const Ticket &ticket : tickets) {
        OrderWidget *orderWidget = new OrderWidget(ticket);
        ui->ordersListLayout->insertWidget(0, orderWidget);
        connect(orderWidget, &OrderWidget::downloadTicketClicked, this, &MainWindow::on_downloadTicket_clicked);
        connect(orderWidget, &OrderWidget::downloadCheckClicked, this, &MainWindow::on_downloadCheck_clicked);
    }
}

void MainWindow::handleTicketCreate(const Ticket &ticket) {
    client->tickets()->getAllTickets();
    ui->route_status->setStyleSheet("color: green");
    ui->route_status->setText("Билет " + QString::number(ticket.getId()) + " куплен");

    client->routes()->getAllRoutes();
}

void MainWindow::handleTicketError(const QString &error) {
    qDebug() << error;
}

void MainWindow::on_downloadTicket_clicked(const Ticket &ticket) {
    Ticket tempTicket = ticket;
    tempTicket.setPassengerName(currentUser.getPerson().getLastName()+" "+currentUser.getPerson().getFirstName());
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save File"),
        QDir::homePath() + "/ticket-"+QString::number(tempTicket.getId())+".txt",
        tr("Text Files (*.txt);;All Files (*.*)")
    );

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << tempTicket.toString() << "\n";
            file.close();
        }
    }
}

void MainWindow::on_downloadCheck_clicked(const Ticket &ticket) {
    Ticket tempTicket = ticket;
    tempTicket.setPassengerName(currentUser.getPerson().getLastName()+" "+currentUser.getPerson().getFirstName());
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save File"),
        QDir::homePath() + "/check-"+QString::number(tempTicket.getId())+".txt",
        tr("Text Files (*.txt);;All Files (*.*)")
    );

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "Operation info from bank for ticket with number " << tempTicket.getId() << "\n";
            file.close();
        }
    }
}

// ================= AUTH ===================
void MainWindow::on_login_btn_clicked() {
    QString username = ui->login_username->text();
    QString password = ui->login_password->text();

    client->auths()->login(username, password);
}

void MainWindow::on_logout_btn_clicked() {
    currentUser.setAuthorized(0);
    client->clearAccessToken();

    if (QFile::exists("cookie")) {
        if (QFile::remove("cookie")) {
            qDebug() << "Файл куки успешно удалён!";
        }
        else {
            qDebug() << "Не удалось удалить файл куки.";
        }
    }
    else {
        qDebug() << "Файл куки не найден.";
    }

    ui->cabinet_textBtn->setText("Войти");
    updateUserData(currentUser);

    clearLayout(ui->ordersListLayout);

    ui->stackedWidget->setCurrentIndex(LOGIN_PAGE);
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
    ui->register_status->setStyleSheet("color: green;");
    ui->register_status->setText("Регистрация прошла успешно\nВойдите в свой аккаунт");
}

void MainWindow::handleLogin(const User &user, const QString &cookie) {
    client->setAccessToken(cookie);
    QFile file("cookie");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    out << cookie.trimmed();
    file.close();

    currentUser = user;
    currentUser.setAuthorized(true);

    updateUserData(user);

    ui->stackedWidget->setCurrentIndex(PROFILE_PAGE);
}

void MainWindow::handleUser(const User &user) {
    updateUserData(user);
    updateServerData();
}

void MainWindow::updateUserData(const User &user) {
    Person person = user.getPerson();

    QString cookie = client->getAccessToken();
    QJsonObject json = JWTDecoder::decodePayload(cookie);

    if (json.contains("role")){
        currentUser.setRole(json["role"].toString());
    }
    else {
        currentUser.setRole("passenger");
    }

    if (currentUser.getRole() == "dispatcher") {
        ui->dispatcher1_menuButton->show();
        ui->dispatcher2_menuButton->show();
    }
    else if (currentUser.getRole() == "admin") {
        ui->dispatcher1_menuButton->show();
        ui->dispatcher2_menuButton->show();
        ui->employees1_menuButton->show();
        ui->employees2_menuButton->show();
        ui->analytics1_menuButton->show();
        ui->analytics2_menuButton->show();
    }
    else {
        ui->dispatcher1_menuButton->hide();
        ui->dispatcher2_menuButton->hide();
        ui->employees1_menuButton->hide();
        ui->employees2_menuButton->hide();
        ui->analytics1_menuButton->hide();
        ui->analytics2_menuButton->hide();
    }


    if (currentUser.isAuthorized()) {
        ui->cabinet_textBtn->setText(person.getLastName() + " " + person.getFirstName());
        ui->userRole_label->setText(roleMatcher(currentUser.getRole()));
        ui->profile_username->setText(user.getUsername());
        ui->profile_lastname->setText(person.getLastName());
        ui->profile_firstname->setText(person.getFirstName());
        ui->profile_middlename->setText(person.getMiddleName());
        ui->profile_email->setText(person.getEmail());
        ui->profile_phone->setText(person.getPhoneNumber());
        ui->profile_birthdate->setText(person.getBirthDate().toString("dd.MM.yyyy"));
    }

    client->tickets()->getAllTickets();
}

void MainWindow::handleAuthError(const QString &error) {
    qDebug() << "Ошибка при обращении к сервису аутентификации и атворизации " << error;
}

void MainWindow::handleRegisterError(const QString &error) {
    ui->register_status->setStyleSheet("color: red;");
    ui->register_status->setText("Ошибка регистрации\n"+error);
}

void MainWindow::handleLoginError(const QString &error) {
    ui->login_status->setStyleSheet("color: red;");
    ui->login_status->setText("Ошибка входа\n"+error);
}

void MainWindow::handleGetUserError(const QString &error) {
    on_logout_btn_clicked();
}

void MainWindow::updateServerData() {
    client->stops()->getAllStops();
    client->paths()->getAllPaths();
    client->routes()->getAllRoutes();
    client->tickets()->getAllTickets();
}

void MainWindow::clearLayout(QLayout *layout){
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

QString MainWindow::roleMatcher(const QString &role) {
    if (role == "passenger") {
        return "пассажир";
    }
    else if (role == "dispatcher") {
        return "диспетчер";
    }
    else if (role == "admin") {
        return "администратор";
    }
    else {
        return "пассажир";
    }
}

void MainWindow::on_schedule1_menuButton_clicked() {
    client->routes()->getAllRoutes();
}


void MainWindow::swapPlaces_btn_clicked() {
    QString temp = ui->tripDeparture->text();
    ui->tripDeparture->setText(ui->tripDestination->text());
    ui->tripDestination->setText(temp);

    temp = ui->tripDeparture2->text();
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