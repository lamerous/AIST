#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "busstop.h"

class BusStopApiClient : public QObject {
    Q_OBJECT

public:
    explicit BusStopApiClient(QObject *parent = nullptr);
    ~BusStopApiClient();

    void addBusStop(const BusStop &busStop);
    void getBusStops();
    void getBusStopById(int id);
    void findBusStopsByName(const QString &name);
    void findBusStopsByAddress(const QString &address);
    void updateBusStop(int id, const BusStop &busStop);
    void deleteBusStop(int id);
    
    void setBaseUrl(const QString &url);

signals:
    void busStopsReceived(const QList<BusStop> &busStops);
    void busStopReceived(const BusStop &busStop);
    void busStopAdded(bool success);
    void busStopUpdated(bool success);
    void busStopDeleted(bool success);
    void errorOccurred(const QString &errorMessage);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager;
    QString baseUrl;

    BusStop jsonToBusStop(const QJsonObject &json);
    QJsonObject busStopToJson(const BusStop &busStop);
    QString getOperationType(QNetworkReply *reply);
};