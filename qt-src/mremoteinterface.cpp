#include "mremoteinterface.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>

MRemoteInterface::MRemoteInterface(QObject *parent) : QObject(parent), transactionActive(false)
{
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
}

void MRemoteInterface::validateWithExistingCredentials()
{
    QString u = settings.getUsername();
    QString p = settings.getPassword();
    if (u.isEmpty() || p.isEmpty()) {
        emit noExistingCredentials();
    } else {
        validate(u, p);
    }
}

QString MRemoteInterface::getUsername()
{
    return settings.getUsername();
}

QString MRemoteInterface::getPassword()
{
    return settings.getPassword();
}

void MRemoteInterface::replyFinished(QNetworkReply *reply)
{
    if (!reply) return;
    if (reply->error()) {
        qDebug() << "reply error: " << reply->errorString();
        emit validationFailed(QString::number(reply->error()));
    } else {
        qDebug() << "reply success: ";
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        QJsonDocument jsonDoc = QJsonDocument::fromBinaryData(reply->readAll());
        QJsonObject response = jsonDoc.object();
        qDebug() << response;
        if (!jsonDoc.isNull()) {
            emit validationSuccess();
        } else {
            emit validationFailed("Could not parse server response.");
        }
    }

    transactionActive = false;
    reply->deleteLater();
}

void MRemoteInterface::validate(QString username, QString password)
{
    if (transactionActive) return; // concurrent access is not supported and probably not needed

    QNetworkRequest request;
    request.setUrl(QUrl(settings.getBaseUrl() + "welcome/verify/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject json;
    json.insert("username", username);
    json.insert("password", password);
    json.insert("version", 1);
    json.insert("request", "verify");
    json.insert("nonce", "asdfasdf");
    json.insert("jump", "jump");

    qDebug() << "json request: " << json;
    transactionActive = true;
    networkManager.post(request, QJsonDocument(json).toJson());
}

void MRemoteInterface::validateRequest(QString username, QString password)
{
    settings.setUsername(username);
    settings.setPassword(password);
    validate(username, password);
}
