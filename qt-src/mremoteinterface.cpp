#include "mremoteinterface.h"
#include "mcamerathread.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QProcess>
#include <QScopeGuard>
#include <QTimer>

const double MRemoteInterface::CURRENT_VERSION = 5.1;

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

MRemoteInterface::MRemoteInterface(QObject *parent) : QObject(parent)
{
    avaliableVersion = "Checking...";
    changelog = "This software is now open source."
                "Please see https://github.com/ironage/maui-gui for updates.";
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(&networkManager, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)), this, SLOT(sslErrors(QNetworkReply*, const QList<QSslError>&)));
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

// Github's public API without a token has a rate limit of 60 requests/hour (per IP)
//    curl -L \
//      -H "Accept: application/vnd.github+json" \
//      -H "X-GitHub-Api-Version: 2022-11-28" \
//      https://api.github.com/repos/ironage/maui-gui/releases/latest

    QNetworkRequest request;
    request.setUrl(QUrl(settings.getVersionEndpoint()));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.github+json");
    request.setRawHeader("X-GitHub-Api-Version", "2022-11-28");
    sendRequest(Request(RequestType::CHANGELOG, request));
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

QString MRemoteInterface::getSoftwareVersion()
{
    return avaliableVersion;
}

QString MRemoteInterface::getChangelog() {
    return changelog;
}

void MRemoteInterface::replyFinished(QNetworkReply *reply)
{
    if (!reply) return;
    auto cleanup = qScopeGuard([&] { reply->deleteLater(); });

    QMutexLocker guard(&queueMutex);
    if (pendingRequests.empty()) return;
    Request request;

    for (size_t i = 0; i < pendingRequests.size(); ++i) {
        if (pendingRequests[i].reply != nullptr && pendingRequests[i].reply == reply) {
            request = pendingRequests[i];
            pendingRequests.erase(pendingRequests.begin() + int(i));
            break;
        }
    }
    guard.unlock();

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
    } else {

        if (reply->attribute(QNetworkRequest::RedirectionTargetAttribute).isValid()) {
            // Qt's automatic redirect policies do not seem to be working, so we do it manually :(
            QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            qDebug() << "redirection: " << redirection << " and " << reply->url().resolved(redirection);
            if (request.numRedirects < 10) {
                request.numRedirects++;
                request.reply = nullptr;
                request.request.setUrl(reply->url().resolved(redirection));
                sendRequest(request);
            } else {
                qDebug() << "Max redirections reached: " << request.numRedirects << "not continuing";
            }
            return;
        }

        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
            qDebug() << "reply success: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        }
        if (!firstResponseProcessed) {
            qDebug() << "connection ok for request: " << request.type;
            firstResponseProcessed = true;
        }
//        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        QByteArray replyBody = reply->readAll();
        qDebug() << "reply: " << replyBody;
        switch (request.type) {
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

void MRemoteInterface::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    qDebug() << "sslErrors encountered: ";
    if (reply) {
        qDebug() << "while processing: " << reply->url();
    }
    for (auto& err : errors) {
        qDebug() << "Error: " << err.errorString();
    }
}

void MRemoteInterface::handleChangelogResponse(QByteArray &data)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (!jsonDoc.isNull()) {
        QJsonObject response = jsonDoc.object();
        QJsonValue softwareVersion = response.value("name");
        QJsonValue log = response.value("body");
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
        qDebug() << "Could not read the software version (from changelog request): " << version;
        return;
    }

    QString result;
    for (const QChar& c : version.toString()) {
      if (c.isDigit() || c == '.') {
        result.append(c);
      }
    }
    double as_double = result.toDouble();
    avaliableVersion = QString::number(as_double, 'f', 1);
    qDebug() << "Version converted is: " << avaliableVersion;
    if (as_double > CURRENT_VERSION) {
        emit validationNewVersionAvailable("Version " + avaliableVersion +
                              " of this software is available!"
                              "\nPlease download the latest version to continue."
                              "\nYou currently are running version " + getDisplayVersion());
    }
    emit softwareVersionChanged();
}

void MRemoteInterface::sendRequest(Request request)
{
    QMutexLocker guard(&queueMutex);
    if (request.reply) return;
    request.request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::RedirectPolicy::ManualRedirectPolicy);
    request.reply = networkManager.get(request.request);
    pendingRequests.push_back(request);
}
