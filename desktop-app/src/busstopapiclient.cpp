#include "busstopapiclient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

BusStopApiClient::BusStopApiClient(QObject *parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , baseUrl("http://localhost:8002")
{
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &BusStopApiClient::onReplyFinished);
}

BusStopApiClient::~BusStopApiClient() {
}

void BusStopApiClient::setBaseUrl(const QString &url) {
    baseUrl = url;
}

void BusStopApiClient::addBusStop(const BusStop &busStop) {
    QUrl url(baseUrl + "/busstops");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json = busStopToJson(busStop);
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    qDebug() << "Sending POST request to:" << url.toString();
    qDebug() << "Request data:" << data;

    qDebug() << QString(doc.toJson());

    networkManager->post(request, data);
}

void BusStopApiClient::getBusStops() {
    QUrl url(baseUrl + "/busstops");
    QNetworkRequest request(url);
    
    qDebug() << "Sending GET request to:" << url.toString();
    
    networkManager->get(request);
}

void BusStopApiClient::getBusStopById(int id) {
    QUrl url(baseUrl + "/busstops/" + QString::number(id));
    QNetworkRequest request(url);
    
    qDebug() << "Sending GET request to:" << url.toString();
    
    networkManager->get(request);
}

void BusStopApiClient::findBusStopsByName(const QString &name) {
    QUrl url(baseUrl + "/busstops/name/" + name);
    QNetworkRequest request(url);
    
    qDebug() << "Sending GET request to:" << url.toString();
    
    networkManager->get(request);
}

void BusStopApiClient::findBusStopsByAddress(const QString &address) {
    QUrl url(baseUrl + "/busstops/address/" + address);
    QNetworkRequest request(url);
    
    qDebug() << "Sending GET request to:" << url.toString();
    
    networkManager->get(request);
}

void BusStopApiClient::updateBusStop(int id, const BusStop &busStop) {
    QUrl url(baseUrl + "/busstops/" + QString::number(id));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json = busStopToJson(busStop);
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    qDebug() << "Sending PUT request to:" << url.toString();
    qDebug() << "Request data:" << data;

    networkManager->put(request, data);
}

void BusStopApiClient::deleteBusStop(int id) {
    QUrl url(baseUrl + "/busstops/" + QString::number(id));
    QNetworkRequest request(url);
    
    qDebug() << "Sending DELETE request to:" << url.toString();
    
    networkManager->deleteResource(request);
}

BusStop BusStopApiClient::jsonToBusStop(const QJsonObject &json) {
    int stopId = json.contains("stop_id") ? json["stop_id"].toInt() : -1;
    QString stopName = json.contains("stop_name") ? json["stop_name"].toString() : "";
    QString stopAddress = json.contains("stop_address") ? json["stop_address"].toString() : "";

    return BusStop(stopId, stopName, stopAddress);
}

QJsonObject BusStopApiClient::busStopToJson(const BusStop &busStop) {
    QJsonObject json;
    json["stop_id"] = busStop.getStopId();
    json["stop_name"] = busStop.getStopName();
    json["stop_address"] = busStop.getStopAddress();
    return json;
}

QString BusStopApiClient::getOperationType(QNetworkReply *reply) {
    QNetworkAccessManager::Operation op = reply->operation();
    switch (op) {
    case QNetworkAccessManager::GetOperation: return "GET";
    case QNetworkAccessManager::PostOperation: return "POST";
    case QNetworkAccessManager::PutOperation: return "PUT";
    case QNetworkAccessManager::DeleteOperation: return "DELETE";
    default: return "UNKNOWN";
    }
}

void BusStopApiClient::onReplyFinished(QNetworkReply *reply) {
    QUrl url = reply->url();
    QString path = url.path();
    QString operation = getOperationType(reply);
    
    qDebug() << "BusStop API - Response from:" << url.toString();
    qDebug() << "Operation:" << operation;
    qDebug() << "Status code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("BusStop API Network error: %1").arg(reply->errorString());
        qDebug() << errorMsg;
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    qDebug() << "Response data:" << response;
    
    QJsonDocument doc = QJsonDocument::fromJson(response);
    
    if (doc.isNull() || !doc.isObject()) {
        QString errorMsg = "Invalid JSON response from BusStop API";
        qDebug() << errorMsg;
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return;
    }

    QJsonObject root = doc.object();
    
    // Проверяем успешность операции
    if (!root.contains("success") || !root["success"].toBool()) {
        QString errorMsg = root.contains("message") ? root["message"].toString() : "Unknown error from BusStop API";
        qDebug() << "BusStop API error:" << errorMsg;
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return;
    }

    // Обработка успешных ответов
    if (operation == "GET") {
        if (path.contains("/busstops/name/") || path.contains("/busstops/address/") || path == "/busstops") {
            // Получение списка остановок
            if (root.contains("busstops") && root["busstops"].isArray()) {
                QList<BusStop> busStops;
                QJsonArray array = root["busstops"].toArray();
                for (const QJsonValue &value : array) {
                    if (value.isObject()) {
                        BusStop busStop = jsonToBusStop(value.toObject());
                        busStops.append(busStop);
                    }
                }
                qDebug() << "Successfully parsed" << busStops.count() << "bus stops";
                emit busStopsReceived(busStops);
            }
        } else if (path.contains("/busstops/") && path.split("/").size() == 3) {
            // Получение одной остановки по ID
            if (root.contains("busstop") && root["busstop"].isObject()) {
                BusStop busStop = jsonToBusStop(root["busstop"].toObject());
                emit busStopReceived(busStop);
            }
        }
    }
    else if (operation == "POST") {
        QString message = root.contains("message") ? root["message"].toString() : "Bus stop added successfully";
        qDebug() << "Bus stop added:" << message;
        emit busStopAdded(true);
    }
    else if (operation == "PUT") {
        QString message = root.contains("message") ? root["message"].toString() : "Bus stop updated successfully";
        qDebug() << "Bus stop updated:" << message;
        emit busStopUpdated(true);
    }
    else if (operation == "DELETE") {
        QString message = root.contains("message") ? root["message"].toString() : "Bus stop deleted successfully";
        qDebug() << "Bus stop deleted:" << message;
        emit busStopDeleted(true);
    }

    reply->deleteLater();
}