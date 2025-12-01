// apiclient.cpp
#include "apiclient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrlQuery>

#define HOST "http://localhost:8000"

// ============== BaseApiClient ==============
BaseApiClient::BaseApiClient(QObject *parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , baseUrl(HOST) 
{
}

BaseApiClient::~BaseApiClient() {
}

void BaseApiClient::setBaseUrl(const QString &url) {
    baseUrl = url;
}

QString BaseApiClient::getBaseUrl() const {
    return baseUrl;
}

QNetworkReply* BaseApiClient::sendRequest(const QString &endpoint, 
                                         const QString &method, 
                                         const QJsonObject &data) {
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = nullptr;
    
    if (method == "GET") {
        reply = networkManager->get(request);
    } else if (method == "POST") {
        QJsonDocument doc(data);
        reply = networkManager->post(request, doc.toJson());
    } else if (method == "PUT") {
        QJsonDocument doc(data);
        reply = networkManager->put(request, doc.toJson());
    } else if (method == "DELETE") {
        reply = networkManager->deleteResource(request);
    }
    
    if (reply) {
        reply->setProperty("endpoint", endpoint);
        reply->setProperty("method", method);
    }
    
    return reply;
}

QJsonObject BaseApiClient::parseJsonResponse(QNetworkReply *reply, bool &success, QString &error) {
    success = false;
    
    if (reply->error() != QNetworkReply::NoError) {
        error = QString("Network error: %1").arg(reply->errorString());
        return QJsonObject();
    }
    
    QByteArray response = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    
    if (doc.isNull()) {
        error = "Invalid JSON response";
        return QJsonObject();
    }
    
    if (!doc.isObject()) {
        error = "Expected JSON object";
        return QJsonObject();
    }
    
    success = true;
    return doc.object();
}

QJsonArray BaseApiClient::parseJsonArrayResponse(QNetworkReply *reply, bool &success, QString &error) {
    success = false;
    
    if (reply->error() != QNetworkReply::NoError) {
        error = QString("Network error: %1").arg(reply->errorString());
        return QJsonArray();
    }
    
    QByteArray response = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    
    if (doc.isNull()) {
        error = "Invalid JSON response";
        return QJsonArray();
    }
    
    if (!doc.isArray()) {
        error = "Expected JSON array";
        return QJsonArray();
    }
    
    success = true;
    return doc.array();
}

// ============== RouteApiClient ==============
RouteApiClient::RouteApiClient(QObject *parent)
    : BaseApiClient(parent) 
{
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &RouteApiClient::handleRoutesResponse);
}

void RouteApiClient::getAllRoutes(int skip, int limit) {
    QUrl url(baseUrl + "/api/routes/");
    QUrlQuery query;
    query.addQueryItem("skip", QString::number(skip));
    query.addQueryItem("limit", QString::number(limit));
    url.setQuery(query);
    
    QNetworkRequest request(url);
    networkManager->get(request)->setProperty("action", "getAll");
}

void RouteApiClient::getRoute(int routeId) {
    QString endpoint = QString("/api/routes/%1").arg(routeId);
    sendRequest(endpoint, "GET")->setProperty("action", "getOne");
}

void RouteApiClient::createRoute(const Route &route) {
    QJsonObject json = routeToJson(route);
    sendRequest("/api/routes/", "POST", json)->setProperty("action", "create");
}

void RouteApiClient::updateRoute(int routeId, const Route &route) {
    QString endpoint = QString("/api/routes/%1").arg(routeId);
    QJsonObject json = routeToJson(route);
    sendRequest(endpoint, "PUT", json)->setProperty("action", "update");
}

void RouteApiClient::deleteRoute(int routeId) {
    QString endpoint = QString("/api/routes/%1").arg(routeId);
    sendRequest(endpoint, "DELETE")->setProperty("action", "delete");
}

Route RouteApiClient::jsonToRoute(const QJsonObject &json) {
    QString routeNumber = json["route_number"].toString();
    QString platformNumber = json["platform_number"].toString();
    
    QDate departureDate = QDate::fromString(json["departure_date"].toString(), "yyyy-MM-dd");
    QTime departureTime = QTime::fromString(json["departure_time"].toString(), "hh:mm:ss");
    QTime arrivalTime = QTime::fromString(json["arrival_time"].toString(), "hh:mm:ss");
    
    int price = json["price"].toInt();
    int seats = json["available_seats"].toInt();
    
    // Создаем путь с заглушками для остановок
    Path path;
    path.setNumber(routeNumber);
    
    // Внимание: в вашем Ticket.h есть проблема - опечатка в параметре конструктора
    // "actialPrice" вместо "actualPrice", но я оставлю как есть
    
    Route route(
        routeNumber,
        platformNumber,
        departureDate,
        departureTime,
        arrivalTime,  // Используем arrival_time как destinationTime
        price,
        seats,
        path
    );
    
    return route;
}

QJsonObject RouteApiClient::routeToJson(const Route &route) {
    QJsonObject json;
    
    json["route_number"] = route.getRouteNumber();
    json["platform_number"] = route.getPlatformNumber();
    json["price"] = route.getPrice();
    json["available_seats"] = route.getSeats();
    json["departure_date"] = route.getDepartureDate().toString("yyyy-MM-dd");
    json["departure_time"] = route.getDepartureTime().toString("hh:mm:ss");
    json["arrival_time"] = route.getDestinationTime().toString("hh:mm:ss");
    json["path_id"] = 1; // Нужно передавать реальный path_id из базы данных
    
    return json;
}

void RouteApiClient::handleRoutesResponse(QNetworkReply *reply) {
    QString action = reply->property("action").toString();
    bool success = false;
    QString error;
    
    if (action == "getAll") {
        QJsonArray array = parseJsonArrayResponse(reply, success, error);
        
        if (success) {
            QList<Route> routes;
            for (const QJsonValue &value : array) {
                if (value.isObject()) {
                    routes.append(jsonToRoute(value.toObject()));
                }
            }
            emit routesReceived(routes);
        } else {
            emit requestError(error);
        }
    }
    else if (action == "getOne") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit routeReceived(jsonToRoute(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "create") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit routeCreated(jsonToRoute(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "update") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit routeUpdated(jsonToRoute(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "delete") {
        if (reply->error() == QNetworkReply::NoError) {
            QString endpoint = reply->property("endpoint").toString();
            QStringList parts = endpoint.split('/');
            if (parts.size() > 0) {
                int routeId = parts.last().toInt();
                emit routeDeleted(routeId);
            }
        } else {
            emit requestError(reply->errorString());
        }
    }
    
    reply->deleteLater();
}

// ============== StopApiClient ==============
StopApiClient::StopApiClient(QObject *parent)
    : BaseApiClient(parent) 
{
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &StopApiClient::handleStopsResponse);
}

void StopApiClient::getAllStops(int skip, int limit) {
    QUrl url(baseUrl + "/api/stops/");
    QUrlQuery query;
    query.addQueryItem("skip", QString::number(skip));
    query.addQueryItem("limit", QString::number(limit));
    url.setQuery(query);
    
    QNetworkRequest request(url);
    networkManager->get(request)->setProperty("action", "getAll");
}

void StopApiClient::getStop(int stopId) {
    QString endpoint = QString("/api/stops/%1").arg(stopId);
    sendRequest(endpoint, "GET")->setProperty("action", "getOne");
}

void StopApiClient::createStop(const BusStop &stop) {
    QJsonObject json = stopToJson(stop);
    sendRequest("/api/stops/", "POST", json)->setProperty("action", "create");
}

void StopApiClient::updateStop(int stopId, const BusStop &stop) {
    QString endpoint = QString("/api/stops/%1").arg(stopId);
    QJsonObject json = stopToJson(stop);
    sendRequest(endpoint, "PUT", json)->setProperty("action", "update");
}

void StopApiClient::deleteStop(int stopId) {
    QString endpoint = QString("/api/stops/%1").arg(stopId);
    sendRequest(endpoint, "DELETE")->setProperty("action", "delete");
}

BusStop StopApiClient::jsonToStop(const QJsonObject &json) {
    QString name = json["stop_name"].toString();
    QString address = json["stop_address"].toString();
    
    // Используем конструктор с двумя параметрами из вашего busstop.h
    BusStop stop(name, address);
    
    return stop;
}

QJsonObject StopApiClient::stopToJson(const BusStop &stop) {
    QJsonObject json;
    json["stop_name"] = stop.getStopName();
    json["stop_address"] = stop.getStopAddress();
    return json;
}

void StopApiClient::handleStopsResponse(QNetworkReply *reply) {
    QString action = reply->property("action").toString();
    bool success = false;
    QString error;
    
    if (action == "getAll") {
        QJsonArray array = parseJsonArrayResponse(reply, success, error);
        
        if (success) {
            QList<BusStop> stops;
            for (const QJsonValue &value : array) {
                if (value.isObject()) {
                    stops.append(jsonToStop(value.toObject()));
                }
            }
            emit stopsReceived(stops);
        } else {
            emit requestError(error);
        }
    }
    else if (action == "getOne") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit stopReceived(jsonToStop(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "create") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit stopCreated(jsonToStop(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "update") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit stopUpdated(jsonToStop(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "delete") {
        if (reply->error() == QNetworkReply::NoError) {
            QString endpoint = reply->property("endpoint").toString();
            QStringList parts = endpoint.split('/');
            if (parts.size() > 0) {
                int stopId = parts.last().toInt();
                emit stopDeleted(stopId);
            }
        } else {
            emit requestError(reply->errorString());
        }
    }
    
    reply->deleteLater();
}

// ============== PathApiClient ==============
PathApiClient::PathApiClient(QObject *parent)
    : BaseApiClient(parent) 
{
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &PathApiClient::handlePathsResponse);
}

void PathApiClient::getAllPaths(int skip, int limit) {
    QUrl url(baseUrl + "/api/paths/");
    QUrlQuery query;
    query.addQueryItem("skip", QString::number(skip));
    query.addQueryItem("limit", QString::number(limit));
    url.setQuery(query);
    
    QNetworkRequest request(url);
    networkManager->get(request)->setProperty("action", "getAll");
}

void PathApiClient::getPath(int pathId) {
    QString endpoint = QString("/api/paths/%1").arg(pathId);
    sendRequest(endpoint, "GET")->setProperty("action", "getOne");
}

void PathApiClient::createPath(const Path &path) {
    QJsonObject json = pathToJson(path);
    sendRequest("/api/paths/", "POST", json)->setProperty("action", "create");
}

void PathApiClient::updatePath(int pathId, const Path &path) {
    QString endpoint = QString("/api/paths/%1").arg(pathId);
    QJsonObject json = pathToJson(path);
    sendRequest(endpoint, "PUT", json)->setProperty("action", "update");
}

void PathApiClient::deletePath(int pathId) {
    QString endpoint = QString("/api/paths/%1").arg(pathId);
    sendRequest(endpoint, "DELETE")->setProperty("action", "delete");
}

Path PathApiClient::jsonToPath(const QJsonObject &json) {
    QString routeNumber = json["route_number"].toString();
    
    // Используем конструктор с одним параметром из вашего path.h
    Path path(routeNumber);
    
    // Пытаемся парсить stops_array
    if (json.contains("stops_array") && json["stops_array"].isObject()) {
        QJsonObject stopsObj = json["stops_array"].toObject();
        // Обработка структурированных данных об остановках
        // (зависит от формата, который вы используете в базе данных)
    }
    
    return path;
}

QJsonObject PathApiClient::pathToJson(const Path &path) {
    QJsonObject json;
    json["route_number"] = path.getNumber();
    
    // Создаем JSON массив остановок
    QJsonArray stopsArray;
    QVector<BusStop> stops = path.getStops();
    for (const auto& stop : stops) {
        QJsonObject stopObj;
        stopObj["name"] = stop.getStopName();
        stopObj["address"] = stop.getStopAddress();
        stopsArray.append(stopObj);
    }
    
    // В FastAPI ожидается объект JSON для stops_array
    QJsonObject stopsObject;
    stopsObject["stops"] = stopsArray;
    json["stops_array"] = stopsObject;
    
    return json;
}

void PathApiClient::handlePathsResponse(QNetworkReply *reply) {
    QString action = reply->property("action").toString();
    bool success = false;
    QString error;
    
    if (action == "getAll") {
        QJsonArray array = parseJsonArrayResponse(reply, success, error);
        
        if (success) {
            QList<Path> paths;
            for (const QJsonValue &value : array) {
                if (value.isObject()) {
                    paths.append(jsonToPath(value.toObject()));
                }
            }
            emit pathsReceived(paths);
        } else {
            emit requestError(error);
        }
    }
    else if (action == "getOne") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit pathReceived(jsonToPath(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "create") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit pathCreated(jsonToPath(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "update") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit pathUpdated(jsonToPath(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "delete") {
        if (reply->error() == QNetworkReply::NoError) {
            QString endpoint = reply->property("endpoint").toString();
            QStringList parts = endpoint.split('/');
            if (parts.size() > 0) {
                int pathId = parts.last().toInt();
                emit pathDeleted(pathId);
            }
        } else {
            emit requestError(reply->errorString());
        }
    }
    
    reply->deleteLater();
}

// ============== TicketApiClient ==============
TicketApiClient::TicketApiClient(QObject *parent)
    : BaseApiClient(parent) 
{
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &TicketApiClient::handleTicketsResponse);
}

void TicketApiClient::getAllTickets(int skip, int limit) {
    QUrl url(baseUrl + "/api/tickets/");
    QUrlQuery query;
    query.addQueryItem("skip", QString::number(skip));
    query.addQueryItem("limit", QString::number(limit));
    url.setQuery(query);
    
    QNetworkRequest request(url);
    networkManager->get(request)->setProperty("action", "getAll");
}

void TicketApiClient::getTicket(int ticketId) {
    QString endpoint = QString("/api/tickets/%1").arg(ticketId);
    sendRequest(endpoint, "GET")->setProperty("action", "getOne");
}

void TicketApiClient::createTicket(const Ticket &ticket) {
    QJsonObject json = ticketToJson(ticket);
    sendRequest("/api/tickets/", "POST", json)->setProperty("action", "create");
}

void TicketApiClient::updateTicket(int ticketId, const Ticket &ticket) {
    QString endpoint = QString("/api/tickets/%1").arg(ticketId);
    QJsonObject json = ticketToJson(ticket);
    sendRequest(endpoint, "PUT", json)->setProperty("action", "update");
}

void TicketApiClient::deleteTicket(int ticketId) {
    QString endpoint = QString("/api/tickets/%1").arg(ticketId);
    sendRequest(endpoint, "DELETE")->setProperty("action", "delete");
}

Ticket TicketApiClient::jsonToTicket(const QJsonObject &json) {
    QString ticketNumber = json["ticket_number"].toString();
    QDate purchaseDate = QDate::fromString(json["purchase_date"].toString(), "yyyy-MM-dd");
    double purchasePrice = json["purchase_price"].toDouble();
    int routeId = json["route_id"].toInt();
    
    // Создаем маршрут-заглушку
    Route stubRoute;
    
    // Используем конструктор из вашего ticket.h
    // Внимание: есть опечатка в параметре - "actialPrice" вместо "actualPrice"
    Ticket ticket(
        stubRoute,
        "Passenger",  // Заглушка для имени пассажира
        ticketNumber,
        "A1",         // Заглушка для номера места
        purchaseDate,
        purchasePrice
    );
    
    return ticket;
}

QJsonObject TicketApiClient::ticketToJson(const Ticket &ticket) {
    QJsonObject json;
    json["ticket_number"] = ticket.getTicketNumber();
    json["purchase_date"] = ticket.getPurchaseDate().toString("yyyy-MM-dd");
    json["purchase_price"] = static_cast<double>(ticket.getActualPrice());
    json["route_id"] = 1; // Заглушка - нужно передавать реальный route_id
    
    return json;
}

void TicketApiClient::handleTicketsResponse(QNetworkReply *reply) {
    QString action = reply->property("action").toString();
    bool success = false;
    QString error;
    
    if (action == "getAll") {
        QJsonArray array = parseJsonArrayResponse(reply, success, error);
        
        if (success) {
            QList<Ticket> tickets;
            for (const QJsonValue &value : array) {
                if (value.isObject()) {
                    tickets.append(jsonToTicket(value.toObject()));
                }
            }
            emit ticketsReceived(tickets);
        } else {
            emit requestError(error);
        }
    }
    else if (action == "getOne") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit ticketReceived(jsonToTicket(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "create") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit ticketCreated(jsonToTicket(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "update") {
        QJsonObject obj = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit ticketUpdated(jsonToTicket(obj));
        } else {
            emit requestError(error);
        }
    }
    else if (action == "delete") {
        if (reply->error() == QNetworkReply::NoError) {
            QString endpoint = reply->property("endpoint").toString();
            QStringList parts = endpoint.split('/');
            if (parts.size() > 0) {
                int ticketId = parts.last().toInt();
                emit ticketDeleted(ticketId);
            }
        } else {
            emit requestError(reply->errorString());
        }
    }
    
    reply->deleteLater();
}

// ============== ApiClient (Main) ==============
ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , routeClient(new RouteApiClient(this))
    , stopClient(new StopApiClient(this))
    , pathClient(new PathApiClient(this))
    , ticketClient(new TicketApiClient(this))
{
}

void ApiClient::setBaseUrl(const QString &url) {
    routeClient->setBaseUrl(url);
    stopClient->setBaseUrl(url);
    pathClient->setBaseUrl(url);
    ticketClient->setBaseUrl(url);
}