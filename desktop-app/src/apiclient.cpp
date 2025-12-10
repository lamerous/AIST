#include "apiclient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrlQuery>

#define HOST "http://localhost:8001"

// ============== BaseApiClient ==============
BaseApiClient::BaseApiClient(QObject *parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , baseUrl(HOST)
{
}

BaseApiClient::~BaseApiClient() {
}

void BaseApiClient::setAccessToken(const QString &token) {
    accessToken = token;
}

QString BaseApiClient::getAccessToken() const {
    return accessToken;
}

void BaseApiClient::clearAccessToken() {
    accessToken.clear();
}

bool BaseApiClient::hasAccessToken() const {
    return !accessToken.isEmpty();
}

void BaseApiClient::setBaseUrl(const QString &url) {
    baseUrl = url;
}

QString BaseApiClient::getBaseUrl() const {
    return baseUrl;
}

QNetworkRequest BaseApiClient::createAuthorizedRequest(const QUrl &url) {
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    if (hasAccessToken()) {
        QString authHeader = QString("Bearer %1").arg(accessToken);
        request.setRawHeader("Authorization", authHeader.toUtf8());
    }
    
    return request;
}

QNetworkReply* BaseApiClient::sendRequest(const QString &endpoint, 
                                         const QString &method, 
                                         const QJsonObject &data) {
    QUrl url(baseUrl + endpoint);
    QNetworkRequest request = createAuthorizedRequest(url);
    
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
        
        if (reply->error() == QNetworkReply::AuthenticationRequiredError ||
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
            emit tokenExpired();
        }
        
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
    
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 401) {
        emit tokenExpired();
        error = "Unauthorized: Invalid or expired token";
        return QJsonObject();
    }
    
    success = true;
    return doc.object();
}

QJsonArray BaseApiClient::parseJsonArrayResponse(QNetworkReply *reply, bool &success, QString &error) {
    success = false;
    
    if (reply->error() != QNetworkReply::NoError) {
        error = QString("Network error: %1").arg(reply->errorString());
        
        if (reply->error() == QNetworkReply::AuthenticationRequiredError ||
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
            emit tokenExpired();
        }
        
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
    
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 401) {
        emit tokenExpired();
        error = "Unauthorized: Invalid or expired token";
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

void RouteApiClient::getAllRoutes(int skip, int limit, QString search) {
    QUrl url(baseUrl + "/api/routes/");
    QUrlQuery query;
    query.addQueryItem("skip", QString::number(skip));
    query.addQueryItem("limit", QString::number(limit));
    if (!search.isEmpty()) {
        query.addQueryItem("search", search);
    }
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
    QString pathNumber = json["path_number"].toString();
    QString platformNumber = json["platform_number"].toString();
    
    QDate departureDate = QDate::fromString(json["departure_date"].toString(), "yyyy-MM-dd");
    QTime departureTime = QTime::fromString(json["departure_time"].toString(), "hh:mm:ss");
    QTime arrivalTime = QTime::fromString(json["arrival_time"].toString(), "hh:mm:ss");
    
    int price = json["price"].toInt();
    int seats = json["available_seats"].toInt();
    
    QVector<BusStop> stops;

    if (json.contains("path") && json["path"].isObject()) {
        QJsonObject pathObj = json["path"].toObject();
        
        if (pathObj.contains("stops_array") && pathObj["stops_array"].isObject()) {
            QJsonObject stopsArrayObj = pathObj["stops_array"].toObject();
            
            if (stopsArrayObj.contains("stops") && stopsArrayObj["stops"].isArray()) {
                QJsonArray stopsArray = stopsArrayObj["stops"].toArray();
                
                for (const QJsonValue &stopValue : stopsArray) {
                    if (stopValue.isObject()) {
                        QJsonObject stopObj = stopValue.toObject();
                        
                        QString name = stopObj["name"].toString();
                        QString address = stopObj["address"].toString();
                                                
                        BusStop stop(name, address);
                        stops.append(stop);
                    }
                }
            } else {
                qDebug() << "No 'stops' array found in stops_array";
            }
        } else {
            qDebug() << "No 'stops_array' found in path";
            
            if (pathObj.contains("stops") && pathObj["stops"].isArray()) {
                QJsonArray stopsArray = pathObj["stops"].toArray();
            }
        }
    } else {
        qDebug() << "No 'path' object found in JSON";
    }

    Route route(
        pathNumber,
        platformNumber,
        departureDate,
        departureTime,
        arrivalTime,
        price,
        seats,
        Path(pathNumber, stops)
    );
    
    if (json.contains("id")) {
        route.setId(json["id"].toInt());
    }
    
    return route;
}

QJsonObject RouteApiClient::routeToJson(const Route &route) {
    QJsonObject json;
    
    json["path_number"] = route.getPathNumber();
    json["platform_number"] = route.getPlatformNumber();
    json["price"] = route.getPrice();
    json["available_seats"] = route.getSeats();
    json["departure_date"] = route.getDepartureDate().toString("yyyy-MM-dd");
    json["departure_time"] = route.getDepartureTime().toString("hh:mm:ss");
    json["arrival_time"] = route.getDestinationTime().toString("hh:mm:ss");
    
    if (route.getId() > 0) {
        json["id"] = route.getId();
    }
    
    Path path = route.getPath();
    if (path.getPathId() > 0) {
        json["path_id"] = path.getPathId();
    }
    
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

void StopApiClient::getAllStops(int skip, int limit, const QString search) {
    QUrl url(baseUrl + "/api/stops/");
    QUrlQuery query;
    query.addQueryItem("skip", QString::number(skip));
    query.addQueryItem("limit", QString::number(limit));
    query.addQueryItem("search", search);
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
    int id = json["id"].toInt();
    QString name = json["stop_name"].toString();
    QString address = json["stop_address"].toString();

    BusStop stop(id, name, address);
    
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

void PathApiClient::getAllPaths(int skip, int limit, const QString search) {
    QUrl url(baseUrl + "/api/paths/");
    QUrlQuery query;
    query.addQueryItem("skip", QString::number(skip));
    query.addQueryItem("limit", QString::number(limit));
    query.addQueryItem("search", search);
    url.setQuery(query);
    
    QNetworkRequest request(url);
    networkManager->get(request)->setProperty("action", "getAll");
}

void PathApiClient::getPath(const QString pathNumber) {
    QString endpoint = QString("/api/paths/%1").arg(pathNumber);
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
    int pathId = json["id"].toInt();
    QString routeNumber = json["route_number"].toString();
    QVector<BusStop> stops;
    
    if (json.contains("stops_array") && json["stops_array"].isObject()) {
        QJsonObject stopsObj = json["stops_array"].toObject();
        
        if (stopsObj.contains("stops") && stopsObj["stops"].isArray()) {
            QJsonArray stopsArray = stopsObj["stops"].toArray();
            
            for (const QJsonValue &stopValue : stopsArray) {
                if (stopValue.isObject()) {
                    QJsonObject stopObj = stopValue.toObject();
                    
                    QString name = stopObj["name"].toString();
                    QString address = stopObj["address"].toString();
                    
                    BusStop stop(name, address);
                    stops.append(stop);
                }
            }
        }
    }
    
    Path path(pathId, routeNumber, stops);
    return path;
}

QJsonObject PathApiClient::pathToJson(const Path &path) {
    QJsonObject json;
    json["route_number"] = path.getNumber();
    
    QJsonArray stopsArray;
    QVector<BusStop> stops = path.getStops();
    for (const auto& stop : stops) {
        QJsonObject stopObj;
        qDebug() << stop.getStopName();
        stopObj["name"] = stop.getStopName();
        stopObj["address"] = stop.getStopAddress();
        stopsArray.append(stopObj);
    }
    
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
    
    Route stubRoute;
    stubRoute.setId(routeId);
    
    Ticket ticket(
        stubRoute,
        "Passenger", 
        ticketNumber,
        "A1",         
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

// ============== AuthApiClient ==============
AuthApiClient::AuthApiClient(QObject *parent)
    : BaseApiClient(parent) 
{
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &AuthApiClient::handleAuthResponse);
}

void AuthApiClient::regist(const QString &username, const QString &last_name, const QString &first_name,
                const QString &middle_name, const QString &phone_number, const QString &email,
                const QDate &birth_date, const QString &password)
{
    QJsonObject json;
    json["username"] = username;
    json["email"] = email;
    json["first_name"] = first_name;
    json["last_name"] = last_name;
    json["middle_name"] = middle_name;
    json["phone_number"] = phone_number;
    json["birth_date"] = birth_date.toString("yyyy-MM-dd");
    json["password"] = password;
    
    sendRequest("/api/auth/register", "POST", json)->setProperty("action", "registration");
}

void AuthApiClient::login(const QString &username, const QString &password) {
    QUrl url(baseUrl + "/api/auth/login");
    QNetworkRequest request(url);
    
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    
    QUrlQuery params;
    params.addQueryItem("username", username);
    params.addQueryItem("password", password);
    // params.addQueryItem("grant_type", "password");
    // params.addQueryItem("client_id", clientId);
    // params.addQueryItem("client_secret", clientSecret);
    // params.addQueryItem("scope", scope);
    
    QByteArray postData = params.query().toUtf8();
    networkManager->post(request, postData)->setProperty("action", "login");
}

void AuthApiClient::get_user() {

}

void AuthApiClient::change_password(const QString &current_password, const QString &new_password) {

}

Person AuthApiClient::jsonToPerson(const QJsonObject& json) {
    
}

QJsonObject AuthApiClient::personToJson(const Person& person) {
}

void AuthApiClient::handleAuthResponse(QNetworkReply *reply) {
    QString action = reply->property("action").toString();
    bool success = false;
    QString error;
    
    if (action == "registration") {
        QJsonArray array = parseJsonArrayResponse(reply, success, error);
        
        if (success) {
            emit userRegistred("User registred");
        } 
        else {
            emit requestError(error);
        }
    }

    else if (action == "login") {
        QJsonObject json = parseJsonResponse(reply, success, error);
        
        if (success) {
            emit userLogedIn(json["access_token"].toString());
        }
        else {
            emit requestError(error);
        }
    }
}

// ============== ApiClient (Main) ==============
ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , routeClient(new RouteApiClient(this))
    , stopClient(new StopApiClient(this))
    , pathClient(new PathApiClient(this))
    , ticketClient(new TicketApiClient(this))
    , authClient(new AuthApiClient(this))
{
}

void ApiClient::setBaseUrl(const QString &url) {
    routeClient->setBaseUrl(url);
    stopClient->setBaseUrl(url);
    pathClient->setBaseUrl(url);
    ticketClient->setBaseUrl(url);
    authClient->setBaseUrl(url);
}

void ApiClient::setAccessToken(const QString &token) {
    routeClient->setAccessToken(token);
    stopClient->setAccessToken(token);
    pathClient->setAccessToken(token);
    ticketClient->setAccessToken(token);
    authClient->setAccessToken(token);
}

QString ApiClient::getAccessToken() const {
    return authClient->getAccessToken();
}

void ApiClient::clearAccessToken() {
    routeClient->clearAccessToken();
    stopClient->clearAccessToken();
    pathClient->clearAccessToken();
    ticketClient->clearAccessToken();
    authClient->clearAccessToken();
}