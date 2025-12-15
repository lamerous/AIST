#include "jwtdecoder.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

QJsonObject JWTDecoder::decode(const QString &token)
{
    QJsonObject result;
    QStringList parts = token.split('.');
    
    if (parts.size() != 3) {
        qWarning() << "Invalid JWT format. Expected 3 parts, got" << parts.size();
        return result;
    }
    
    result["header"] = parseJsonFromBase64(parts[0]);
    result["payload"] = parseJsonFromBase64(parts[1]);
    
    return result;
}

QJsonObject JWTDecoder::decodeHeader(const QString &token)
{
    QStringList parts = token.split('.');
    
    if (parts.size() < 1) {
        return QJsonObject();
    }
    
    return parseJsonFromBase64(parts[0]);
}

QJsonObject JWTDecoder::decodePayload(const QString &token)
{
    QStringList parts = token.split('.');
    
    if (parts.size() < 2) {
        return QJsonObject();
    }
    
    return parseJsonFromBase64(parts[1]);
}

QByteArray JWTDecoder::base64UrlDecode(const QString &data)
{
    QString base64 = data;
    base64.replace('-', '+').replace('_', '/');
    
    switch (base64.size() % 4) {
        case 2: base64 += "=="; break;
        case 3: base64 += "="; break;
        default: break;
    }
    
    return QByteArray::fromBase64(base64.toUtf8());
}

QJsonObject JWTDecoder::parseJsonFromBase64(const QString &base64Str)
{
    QByteArray decoded = base64UrlDecode(base64Str);
    QJsonDocument doc = QJsonDocument::fromJson(decoded);
    
    if (!doc.isObject()) {
        qWarning() << "Failed to parse JSON from base64";
        return QJsonObject();
    }
    
    return doc.object();
}