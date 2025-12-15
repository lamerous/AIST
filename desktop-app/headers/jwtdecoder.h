#pragma once

#include <QString>
#include <QJsonObject>

class JWTDecoder
{
public:
    JWTDecoder() = default;

    static QJsonObject decode(const QString &token);
    
    static QJsonObject decodeHeader(const QString &token);
    
    static QJsonObject decodePayload(const QString &token);

private:
    static QByteArray base64UrlDecode(const QString &data);
    static QJsonObject parseJsonFromBase64(const QString &base64Str);
};