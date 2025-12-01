#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "route.h"

class ApiClient : public QObject {
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient();

    void addRoute(const Route &route);
    void getRoutes();
    void setBaseUrl(const QString &url);

signals:
    void routesReceived(const QList<Route> &routes);
    void routeAdded(bool success);
    void errorOccurred(const QString &errorMessage);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager;
    QString baseUrl;

    Route jsonToRoute(const QJsonObject &json);
    QJsonObject routeToJson(const Route &route);
};