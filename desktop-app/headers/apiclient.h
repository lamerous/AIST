#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkCookieJar>
#include <QNetworkCookie>

#include "route.h"
#include "path.h"
#include "busstop.h"
#include "ticket.h"
#include "person.h"
#include "user.h"

// Базовый класс для работы с API
class BaseApiClient : public QObject {
    Q_OBJECT

public:
    explicit BaseApiClient(QObject *parent = nullptr);
    virtual ~BaseApiClient();

    void setBaseUrl(const QString &url);
    QString getBaseUrl() const;

    void setAccessToken(const QString &token);
    QString getAccessToken() const;
    void clearAccessToken();
    bool hasAccessToken() const;

protected:
    QNetworkReply* sendRequest(const QString &endpoint, 
                              const QString &method, 
                              const QJsonObject &data = QJsonObject());
    
    QJsonObject parseJsonResponse(QNetworkReply *reply, bool &success, QString &error);
    QJsonArray parseJsonArrayResponse(QNetworkReply *reply, bool &success, QString &error);
    
    QNetworkRequest createAuthorizedRequest(const QUrl &url);

    QNetworkAccessManager *networkManager;
    QString baseUrl;
    QString accessToken;

signals:
    void requestError(const QString &errorMessage);
    void tokenExpired();
};

// Клиент для работы с маршрутами
class RouteApiClient : public BaseApiClient {
    Q_OBJECT

public:
    explicit RouteApiClient(QObject *parent = nullptr);
    
    void getAllRoutes(int skip = 0, int limit = 100, const QString &search = "",
                    const QString &start_stop = "", const QString &end_stop = "");
    void getRoute(int routeId);
    void createRoute(const Route &route);
    void updateRoute(int routeId, const Route &route);
    void deleteRoute(int routeId);

signals:
    void routesReceived(const QList<Route> &routes);
    void routeReceived(const Route &route);
    void routeCreated(const Route &route);
    void routeUpdated(const Route &route);
    void routeDeleted(int routeId);

private slots:
    void handleRoutesResponse(QNetworkReply *reply);
    
private:
    Route jsonToRoute(const QJsonObject &json);
    QJsonObject routeToJson(const Route &route);
};

// Клиент для работы с остановками
class StopApiClient : public BaseApiClient {
    Q_OBJECT

public:
    explicit StopApiClient(QObject *parent = nullptr);
    
    void getAllStops(int skip = 0, int limit = 100, const QString search = "");
    void getStop(int stopId);
    void createStop(const BusStop &stop);
    void updateStop(int stopId, const BusStop &stop);
    void deleteStop(int stopId);

signals:
    void stopsReceived(const QList<BusStop> &stops);
    void stopReceived(const BusStop &stop);
    void stopCreated(const BusStop &stop);
    void stopUpdated(const BusStop &stop);
    void stopDeleted(int stopId);

private slots:
    void handleStopsResponse(QNetworkReply *reply);
    
private:
    BusStop jsonToStop(const QJsonObject &json);
    QJsonObject stopToJson(const BusStop &stop);
};

// Клиент для работы с путями
class PathApiClient : public BaseApiClient {
    Q_OBJECT

public:
    explicit PathApiClient(QObject *parent = nullptr);
    
    void getAllPaths(int skip = 0, int limit = 100, const QString search = "");
    void getPath(const QString pathNumber);
    void createPath(const Path &path);
    void updatePath(int pathId, const Path &path);
    void deletePath(int pathId);

signals:
    void pathsReceived(const QList<Path> &paths);
    void pathReceived(const Path &path);
    void pathCreated(const Path &path);
    void pathUpdated(const Path &path);
    void pathDeleted(int pathId);

private slots:
    void handlePathsResponse(QNetworkReply *reply);
    
private:
    Path jsonToPath(const QJsonObject &json);
    QJsonObject pathToJson(const Path &path);
};

// Клиент для работы с билетами
class TicketApiClient : public BaseApiClient {
    Q_OBJECT

public:
    explicit TicketApiClient(QObject *parent = nullptr);
    
    void getAllTickets(int skip = 0, int limit = 100);
    void getTicket(int ticketId);
    void createTicket(int routeId);
    void updateTicket(int ticketId, const Ticket &ticket);
    void deleteTicket(int ticketId);

signals:
    void ticketsReceived(const QList<Ticket> &tickets);
    void ticketReceived(const Ticket &ticket);
    void ticketCreated(const Ticket &ticket);
    void ticketUpdated(const Ticket &ticket);
    void ticketDeleted(int ticketId);

private slots:
    void handleTicketsResponse(QNetworkReply *reply);
    
private:
    Ticket jsonToTicket(const QJsonObject &json);
    QJsonObject ticketToJson(const Ticket &ticket);
};

class AuthApiClient : public BaseApiClient {
    Q_OBJECT

public:
    explicit AuthApiClient(QObject *parent = nullptr);

    void regist(const QString &username, const QString &last_name, const QString &first_name,
                const QString &middle_name, const QString &phone_number, const QString &email,
                const QDate &birth_date, const QString &password);
    void login(const QString &username, const QString &password);
    void get_user();
    void change_password(const QString &current_password, const QString &new_password);

signals:
    void userRegistred(const QString &message);
    void userLogedIn(const User &user, const QString &cookie);
    void getUser(const User &user);
    void registerError(const QString &error);
    void loginError(const QString &error);
    void getUserError(const QString &error);

private slots:
    void handleAuthResponse(QNetworkReply *reply);

private:
    User jsonToUser(const QJsonObject &json);
    QJsonObject userToJson(const User &user);
};

// Главный клиент, объединяющий все API
class ApiClient : public QObject {
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);
    
    // Геттеры для конкретных клиентов
    RouteApiClient* routes() const { return routeClient; }
    StopApiClient* stops() const { return stopClient; }
    PathApiClient* paths() const { return pathClient; }
    TicketApiClient* tickets() const { return ticketClient; }
    AuthApiClient* auths() const { return authClient; }
    
    void setBaseUrl(const QString &url);

    void setAccessToken(const QString &token);
    QString getAccessToken() const;
    void clearAccessToken();

private:
    RouteApiClient *routeClient;
    StopApiClient *stopClient;
    PathApiClient *pathClient;
    TicketApiClient *ticketClient;
    AuthApiClient *authClient;
};