#include "apiclient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

#define HOST "http://localhost:8001"

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , baseUrl(HOST) 
{
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &ApiClient::onReplyFinished);
}

ApiClient::~ApiClient() {
}

void ApiClient::setBaseUrl(const QString &url) {
    baseUrl = url;
}

void ApiClient::addRoute(const Route &route) {
    QUrl url(baseUrl + "/routes");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json = routeToJson(route);
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    qDebug() << "Sending POST request to:" << url.toString();
    qDebug() << "Request data:" << data;

    networkManager->post(request, data);
}

void ApiClient::getRoutes()
{
    QUrl url(baseUrl + "/routes");
    QNetworkRequest request(url);
    
    qDebug() << "Sending GET request to:" << url.toString();
    
    networkManager->get(request);
}

Route ApiClient::jsonToRoute(const QJsonObject &json) {
    QString routeNumber = json.contains("route_number") ? json["route_number"].toString() : "";
    QString platformNumber = json.contains("platform_number") ? json["platform_number"].toString() : "";
    QString departurePlace = json.contains("departure_place") ? json["departure_place"].toString() : "";
    QString destinationPlace = json.contains("destination_place") ? json["destination_place"].toString() : "";
    
    QDate departureDate;
    QString dateStr = json.contains("departure_date") ? json["departure_date"].toString() : "";
    if (!dateStr.isEmpty()) {
        departureDate = QDate::fromString(dateStr, "yyyy-MM-dd");
        if (!departureDate.isValid()) {
            departureDate = QDate::fromString(dateStr, "dd.MM.yyyy");
        }
    }
    if (!departureDate.isValid()) {
        departureDate = QDate::currentDate();
    }
    
    QTime departureTime = json.contains("departure_time") ? 
                         QTime::fromString(json["departure_time"].toString(), "hh:mm") : QTime(0, 0);
    QTime destinationTime = json.contains("destination_time") ? 
                           QTime::fromString(json["destination_time"].toString(), "hh:mm") : QTime(0, 0);
    
    int price = json.contains("price") ? json["price"].toInt() : 0;
    int seats = json.contains("seats") ? json["seats"].toInt() : 0;

    return Route(
        routeNumber,
        platformNumber,
        departurePlace,
        destinationPlace,
        departureDate,
        departureTime,
        destinationTime,
        price,
        seats
    );
}

QJsonObject ApiClient::routeToJson(const Route &route) {
    QJsonObject json;
    
    json["route_number"] = route.getRouteNumber();
    json["platform_number"] = route.getPlatformNumber();
    json["departure_place"] = route.getDeparturePlace();
    json["destination_place"] = route.getDestinationPlace();
    
    json["departure_date"] = route.getDepartureDate().toString("yyyy-MM-dd");
    
    json["departure_time"] = route.getDepartureTime().toString("hh:mm");
    json["destination_time"] = route.getDestinationTime().toString("hh:mm");
    
    json["price"] = route.getPrice();
    json["seats"] = route.getSeats();
    
    return json;
}

void ApiClient::onReplyFinished(QNetworkReply *reply) {
    QUrl url = reply->url();
    QString path = url.path();
    QNetworkAccessManager::Operation operation = reply->operation();
    
    qDebug() << "Response received from:" << url.toString();
    qDebug() << "Operation:" << operation;
    qDebug() << "Status code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("Network error: %1").arg(reply->errorString());
        qDebug() << errorMsg;
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    qDebug() << "Response data:" << response;
    
    QJsonDocument doc = QJsonDocument::fromJson(response);
    
    if (doc.isNull()) {
        QString errorMsg = "Invalid JSON response";
        qDebug() << errorMsg;
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return;
    }

    if (!doc.isObject()) {
        QString errorMsg = "Expected JSON object in response";
        qDebug() << errorMsg;
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return;
    }

    QJsonObject root = doc.object();
    
    if (path.contains("/routes")) {
        if (operation == QNetworkAccessManager::GetOperation) {
            QList<Route> routes;
            
            if (root.contains("success") && root["success"].toBool()) {
                if (root.contains("routes") && root["routes"].isArray()) {
                    QJsonArray array = root["routes"].toArray();
                    for (const QJsonValue &value : array) {
                        if (value.isObject()) {
                            Route route = jsonToRoute(value.toObject());
                            routes.append(route);
                        }
                    }
                    qDebug() << "Successfully parsed" << routes.count() << "routes";
                    emit routesReceived(routes);
                } else {
                    QString errorMsg = "No 'routes' array found in response";
                    qDebug() << errorMsg;
                    emit errorOccurred(errorMsg);
                }
            } else {
                QString errorMsg = "Server returned error";
                if (root.contains("message")) {
                    errorMsg = root["message"].toString();
                }
                qDebug() << errorMsg;
                emit errorOccurred(errorMsg);
            }
        }
        else if (operation == QNetworkAccessManager::PostOperation) {
            bool success = root.contains("success") && root["success"].toBool();
            QString message = root.contains("message") ? root["message"].toString() : "Unknown error";
            
            if (success) {
                qDebug() << "Route added successfully:" << message;
                emit routeAdded(true);
            } else {
                qDebug() << "Failed to add route:" << message;
                emit errorOccurred(message);
            }
        }
    }

    reply->deleteLater();
}