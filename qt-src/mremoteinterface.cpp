#include "mremoteinterface.h"

#include "qblowfish.h"

#include <QCoreApplication>
#include <QByteArray>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>

static QByteArray sharedKey = "6mC4zR5SVzug3uiB9L42I164Wn640wt1";
const double MRemoteInterface::CURRENT_VERSION = 4.0;

QJsonArray toJsonArray(QByteArray ba) {
    QJsonArray array;
    for (int i = 0; i < ba.size(); i++) {
        array.append(QJsonValue(static_cast<unsigned char>(ba.at(i))));
    }
    return array;
}

QByteArray fromJsonArray(QJsonArray ja) {
    QByteArray ba;
    for (int i = 0; i < ja.size(); i++) {
        ba.append(ja.at(i).toInt());
    }
    return ba;
}

MRemoteInterface::MRemoteInterface(QObject *parent) : QObject(parent), transactionActive(false)
{
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
}

QString MRemoteInterface::getDisplayVersion()
{
    return QString::number(CURRENT_VERSION, 'g', 2);
}

void MRemoteInterface::setLocalSetting(QString key, QString value)
{
    settings.setRaw(key, value);
}

QString MRemoteInterface::getLocalSetting(QString key)
{
    return settings.getRaw(key);
}

void MRemoteInterface::validateWithExistingCredentials()
{
    QString u = settings.getUsername();
    QString p = settings.getPassword();
    if (u.isEmpty() || p.isEmpty()) {
        emit noExistingCredentials();
    } else {
        validate(u, p, "verify");
    }
}

void MRemoteInterface::validateRequest(QString username, QString password)
{
    settings.setUsername(username);
    settings.setPassword(password);
    emit usernameChanged();
    emit passwordChanged();
    validate(username, password, "verify");
}

void MRemoteInterface::finishSession()
{
    QString u = settings.getUsername();
    QString p = settings.getPassword();
    if (u.isEmpty() || p.isEmpty()) {
        emit noExistingCredentials();
    } else {
        validate(u, p, "finish");
    }
}

void MRemoteInterface::doUpdate()
{
    QProcess::startDetached("maintenancetool.exe", QStringList());
    connect(&killTimer, SIGNAL(timeout()), this, SLOT(die()));
    killTimer.setSingleShot(true);
    killTimer.start(2500);
}

void MRemoteInterface::die() {
    QCoreApplication::quit();
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

    qDebug() << "reply code: " + QString::number(reply->error());
    if (reply->error()) {
        QString errorString = reply->errorString();
        QString replyBody = reply->readAll();
        QString message = "Please check your internet connection.";
        message += "\n[code " + QString::number(reply->error()) + "]";
        //message += "\n" + errorString;

        qDebug() << "reply error:" << replyBody;
        qDebug() << "error string: " << errorString;
        if (reply->error() == QNetworkReply::InternalServerError) {
            message = "Authentication problem.";
            message += "\n[code " + QString::number(reply->error()) + "]";
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
            if (softwareVersion.isUndefined()) {
                emit validationFailed("Could not read the software version.\nInstalling the latest version may fix this problem.");
            } else if (softwareVersion.toDouble() > CURRENT_VERSION) {
                emit validationNewVersionAvailable("Version " + QString::number(softwareVersion.toDouble()) +
                                      " of this software is available!"
                                      "\nPlease download the latest version to continue."
                                      "\nYou currently are running version " + getDisplayVersion());
            } else if (jump.isUndefined() || !jump.isArray()
                       || nonce.isUndefined() || !nonce.isArray()
                       || status.isUndefined() || !status.isArray()
                       || name.isUndefined() || !name.isArray()
                       || version.isUndefined()) {
                emit validationFailed("Unexpected response from the server!"
                                      "\nTry updating the software to the latest version."
                                      "\nYou currently are running version " + getDisplayVersion());
            } else {
                QByteArray nonceR = fromJsonArray(nonce.toArray());
                QBlowfish bf(sharedKey);
                bf.setPaddingEnabled(true);
                nonceR = bf.decrypted(nonceR);
                if (nonceR.isEmpty()) {
                    emit validationFailed("Could not read server encryption details.");
                } else {
                    QString nameR = decryptFromServer(fromJsonArray(name.toArray()), sharedKey, nonceR);
                    QString statusR = decryptFromServer(fromJsonArray(status.toArray()), sharedKey, nonceR);
                    QString jumpR = decryptFromServer(fromJsonArray(jump.toArray()), sharedKey, nonceR);
                    if (nameR != settings.getUsername() || jumpR != "Omnia cum pretio") {
                        emit validationFailed("Encryption failure.");
                    } else if (statusR == "invalid") {
                        emit validationBadCredentials();
                    } else if (statusR == "expired") {
                        emit validationAccountExpired();
                    } else if (statusR == "multiple_sessions") {
                        emit multipleSessionsDetected();
                    } else if (statusR == "valid") {
                        emit validationSuccess();
                    } else if (statusR == "finished") {
                        emit sessionFinished();
                    } else {
                        emit validationFailed("Unexpected response from the server!"
                                              "\nTry updating the software to the latest version."
                                              "\nYou currently are running version " + getDisplayVersion());
                    }
                }
            }
        } else {
            emit validationFailed("Could not parse server response.");
        }
    }

    reply->deleteLater();
}

void MRemoteInterface::validate(QString username, QString password, QString method)
{
    if (transactionActive) return; // concurrent access is not supported and probably not needed

    QNetworkRequest request;
    request.setUrl(QUrl(settings.getBaseUrl() + "welcome/verify/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString nonceQString = MSettings::getRandomString(16);
    qDebug() << "sending nonce: " << nonceQString;
    std::string stdNonce = nonceQString.toStdString();
    QByteArray nonce = QByteArray::fromRawData(stdNonce.c_str(), nonceQString.size());

    QJsonObject json;
    json.insert("username", encryptForServer(username, nonce, sharedKey));
    json.insert("password", encryptForServer(password, nonce, sharedKey));
    json.insert("version", 1);
    json.insert("request", encryptForServer(method, nonce, sharedKey));
    json.insert("nonce", encryptForServer(nonceQString, sharedKey));
    json.insert("jump", encryptForServer("Abyssus abyssum invocat", nonce, sharedKey));

    qDebug() << "json request: " << json;
    transactionActive = true;
    networkManager.post(request, QJsonDocument(json).toJson());
}

QJsonArray MRemoteInterface::encryptForServer(QString value, QByteArray key, QByteArray key2)
{
    std::string stdValue = value.toStdString();
    QByteArray storable = QByteArray::fromRawData(stdValue.c_str(), value.size());
    if (!key.isEmpty()) {
        QBlowfish bf(key);
        bf.setPaddingEnabled(true);
        storable = bf.encrypted(storable);
    }
    if (!key2.isEmpty()) {
        QBlowfish bf(key2);
        bf.setPaddingEnabled(true);
        storable = bf.encrypted(storable);
    }
    return toJsonArray(storable);
}

QString MRemoteInterface::decryptFromServer(QByteArray value, QByteArray key, QByteArray key2)
{
    if (!key.isEmpty()) {
        QBlowfish bf(key);
        bf.setPaddingEnabled(true);
        value = bf.decrypted(value);
    }
    if (!key2.isEmpty()) {
        QBlowfish bf(key2);
        bf.setPaddingEnabled(true);
        value = bf.decrypted(value);
    }
    return QString::fromStdString(value.toStdString());
}
