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
    transactionActive = false;
    if (!reply) return;

    if (reply->error()) {
        QString errorString = reply->errorString();
        QString replyBody = reply->readAll();
        QString message = "Please check your internet connection.";
        message += "\n[code " + QString::number(reply->error()) + "]";
        message += "\n" + replyBody;
        qDebug() << "reply error:" << replyBody << errorString;
        if (reply->error() == QNetworkReply::InternalServerError) {
            message = "Authentication problem.\n" + replyBody;
        }
        emit validationFailed(message);
    } else {
        qDebug() << "reply success: ";
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        QByteArray replyBody = reply->readAll();
        qDebug() << "body: " << replyBody;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(replyBody);
        if (!jsonDoc.isNull()) {
            QJsonObject response = jsonDoc.object();
            QJsonValue softwareVersion = response.value("software_version");
            QJsonValue jump = response.value("jump");
            QJsonValue nonce = response.value("nonce");
            QJsonValue status = response.value("status");
            QJsonValue name = response.value("username");
            QJsonValue version = response.value("version");
            int currentVersion = 2;
            if (softwareVersion.isUndefined()) {
                emit validationFailed("Could not read the software version.\nInstalling the latest version may fix this problem.");
            } else if (softwareVersion.toInt() > currentVersion) {
                emit validationNewVersionAvailable("Version " + QString::number(softwareVersion.toInt()) +
                                      " of this software is available!"
                                      "\nPlease download the latest version to continue."
                                      "\nYou currently are running version " + QString::number(currentVersion));
            } else if (jump.isUndefined() || jump.toString().isEmpty()
                       || nonce.isUndefined() || nonce.toString().isEmpty()
                       || status.isUndefined() || status.toString().isEmpty()
                       || name.isUndefined() || name.toString().isEmpty()
                       || version.isUndefined()) {
                emit validationFailed("Unexpected response from the server!"
                                      "\nTry updating the software to the latest version."
                                      "\nYou currently are running version " + QString::number(currentVersion));
            } else {
                if (name.toString() != settings.getUsername()
                        || jump.toString() != "Omnia cum pretio") {
                    emit validationFailed("Encryption failure.");
                } else if (status.toString() == "invalid") {
                    emit validationBadCredentials();
                } else if (status.toString() == "expired") {
                    emit validationAccountExpired();
                } else if (status.toString() == "valid") {
                    emit validationSuccess();
                }
            }
        } else {
            emit validationFailed("Could not parse server response.");
        }
    }

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
