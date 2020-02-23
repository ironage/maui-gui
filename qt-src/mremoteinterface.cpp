#include "mremoteinterface.h"
#include "mcamerathread.h"

#include "qblowfish.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QByteArray>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QTimer>

static QByteArray sharedKey = "6mC4zR5SVzug3uiB9L42I164Wn640wt1";
const double MRemoteInterface::CURRENT_VERSION = 4.8;

QString Metric::processFromSetupState(int setupState)
{
    if (setupState == CameraTask::SetupState::NONE) {
        return "none";
    } else if (setupState == CameraTask::SetupState::NORMAL_ROI) {
        return "diameter";
    } else if (setupState == CameraTask::SetupState::VELOCITY_ROI) {
        return "velocity";
    } else if (setupState == (CameraTask::SetupState::ALL)) {
        return "diameter and velocity";
    }
    return QString("unknown ") + QString::number(setupState);
}

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

MRemoteInterface::MRemoteInterface(QObject *parent) : QObject(parent), changelog("Fetching changes...")
{
    avaliableVersion = "Checking...";
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

void MRemoteInterface::requestChangelog()
{
    QNetworkRequest request;
    request.setUrl(QUrl(settings.getBaseUrl() + "welcome/changelog/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json.insert("request", "changelog");

    requestQueue.enqueue(Request(RequestType::CHANGELOG, request, json));
    processNextRequest();
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

void MRemoteInterface::videoStateChange(QString playbackState, QString readSrcExtension, int frameIndex, int duration, QString source, int setupState)
{
    metrics.enqueue(Metric(playbackState, readSrcExtension, frameIndex, duration, source, setupState));
}

void MRemoteInterface::validateRequest(QString username, QString password)
{
    settings.setUsername(username);
    settings.setPassword(password);
    emit usernameChanged();
    emit passwordChanged();
    validate(username, password, "verify");
}

void MRemoteInterface::changeExistingCredentials(QString username, QString password)
{
    settings.setUsername(username);
    settings.setPassword(password);
    emit usernameChanged();
    emit passwordChanged();
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

QString MRemoteInterface::getSoftwareVersion()
{
    return avaliableVersion;
}

QString MRemoteInterface::getChangelog() {
    return changelog;
}

void MRemoteInterface::replyFinished(QNetworkReply *reply)
{
    QTimer::singleShot(0, this, SLOT(processNextRequest())); // queue up the next request after this
    if (requestQueue.isEmpty()) return;
    Request request = requestQueue.dequeue();
    if (!reply) return;
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "reply code: " + QString::number(reply->error());
    }
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
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
            qDebug() << "reply success: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        }
        if (!firstResponseProcessed) {
            qDebug() << "connection ok for request: " << request.type;
            firstResponseProcessed = true;
        }
        //qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        QByteArray replyBody = reply->readAll();
        //qDebug() << "reply: " << replyBody;
        switch (request.type) {
        case RequestType::VERIFY:
            handleVerifyResponse(replyBody);
            break;
        case RequestType::VERSION:
            handleVersionResponse(replyBody);
            break;
        case RequestType::CHANGELOG:
            handleChangelogResponse(replyBody);
            break;
        case RequestType::NONE:
            qDebug() << "Error: cannot handle response of NONE!";
            break;
        default:
            qDebug() << "Unhandled default network response!";
            break;
        }
    }
}

void MRemoteInterface::validate(QString username, QString password, QString method)
{
    QNetworkRequest request;
    request.setUrl(QUrl(settings.getBaseUrl() + "welcome/verify/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString nonceQString = MSettings::getRandomString(16);
    std::string stdNonce = nonceQString.toStdString();
    QByteArray nonce = QByteArray::fromRawData(stdNonce.c_str(), nonceQString.size());

    QJsonObject json;
    // version 1
    json.insert("username", encryptForServer(username, nonce, sharedKey));
    json.insert("password", encryptForServer(password, nonce, sharedKey));
    json.insert("version", 2);
    json.insert("request", encryptForServer(method, nonce, sharedKey));
    json.insert("nonce", encryptForServer(nonceQString, sharedKey));
    json.insert("jump", encryptForServer("Abyssus abyssum invocat", nonce, sharedKey));

    // version 2 additions
    json.insert("maui_version", QString::number(MRemoteInterface::CURRENT_VERSION));
    json.insert("metrics_version", 1);
    QJsonArray json_metrics;

    if (settings.getMetricsEnabled()) {
        while(!metrics.empty()) {
            Metric m = metrics.dequeue();
            QJsonObject mjson;
            mjson.insert("event", m.event);
            mjson.insert("type", m.extension);
            mjson.insert("frames", m.frameIndex);
            mjson.insert("process", m.process);
            mjson.insert("processing_milliseconds", m.duration);
            mjson.insert("timestamp", m.created.toSecsSinceEpoch()); // utc
            mjson.insert("name_hash", // sha3_256 is 64 long
                         QString::fromLocal8Bit(QCryptographicHash::hash(m.source.toLocal8Bit(), QCryptographicHash::Sha3_256).toBase64()));
            json_metrics.append(mjson);
        }
    }
    json.insert("metrics", json_metrics);

    requestQueue.enqueue(Request(RequestType::VERIFY, request, json));
    processNextRequest();
}

void MRemoteInterface::handleVerifyResponse(QByteArray &data)
{
    //qDebug() << "body: " << data;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (!jsonDoc.isNull()) {
        QJsonObject response = jsonDoc.object();
        QJsonValue softwareVersion = response.value("software_version");
        QJsonValue jump = response.value("jump");
        QJsonValue nonce = response.value("nonce");
        QJsonValue status = response.value("status");
        QJsonValue name = response.value("username");
        QJsonValue version = response.value("version");
        processLatestVersion(softwareVersion);
        if (jump.isUndefined() || !jump.isArray()
                   || nonce.isUndefined() || !nonce.isArray()
                   || status.isUndefined() || !status.isArray()
                   || name.isUndefined() || !name.isArray()
                   || version.isUndefined()) {
            emit validationFailed("Unexpected response from the server!"
                                  "\nTry updating the software to the latest version."
                                  "\nYou currently are running version " + getDisplayVersion());
            return;
        }

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
            if (version.toInt() >= 2) {
                QString metricsResponse = response.value("stats").toString();
                if (!metricsResponse.startsWith("Metrics success")) {
                    qDebug() << "Metrics reply: " << metricsResponse;
                }
            }
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
    } else {
        emit validationFailed("Could not parse server response.");
    }
}

void MRemoteInterface::handleVersionResponse(QByteArray &data)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (!jsonDoc.isNull()) {
        QJsonObject response = jsonDoc.object();
        QJsonValue softwareVersion = response.value("software_version");
        processLatestVersion(softwareVersion);
    } else {
        qDebug() << "Could not parse server response (version response)."; // we consider this not fatal
    }
}

void MRemoteInterface::handleChangelogResponse(QByteArray &data)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (!jsonDoc.isNull()) {
        QJsonObject response = jsonDoc.object();
        QJsonValue softwareVersion = response.value("software_version");
        QJsonValue log = response.value("changelog");
        processLatestVersion(softwareVersion);
        if (log.isUndefined()) {
            changelog = "Error: could not read changelog from server.";
        } else {
            changelog = log.toString();
        }
        emit changelogChanged();
    } else {
        qDebug() << "Could not parse server response (changelog response)."; // we consider this not fatal
    }
}

void MRemoteInterface::processLatestVersion(QJsonValue &version)
{
    if (version.isUndefined()) {
        qDebug() << "Could not read the software version (from changelog request)";
    } else if (version.toDouble() > CURRENT_VERSION) {
        emit validationNewVersionAvailable("Version " + QString::number(version.toDouble()) +
                              " of this software is available!"
                              "\nPlease download the latest version to continue."
                              "\nYou currently are running version " + getDisplayVersion());
    }
    avaliableVersion = QString::number(version.toDouble());
    emit softwareVersionChanged();
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

void MRemoteInterface::processNextRequest()
{
    if (requestQueue.isEmpty()) return;
    Request &request = requestQueue.head();
    if (request.active) return; // already in progress, we'll queue up the next request when it's finished
    request.active = true;
    networkManager.post(request.request, QJsonDocument(request.postData).toJson());
}
