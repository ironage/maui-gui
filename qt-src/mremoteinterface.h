#ifndef MREMOTEINTERFACE_H
#define MREMOTEINTERFACE_H

#include "msettings.h"

#include <QJsonObject>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QTimer>
#include <QQueue>
#include <QUrl>

#include <vector>

class MRemoteInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString softwareVersion READ getSoftwareVersion NOTIFY softwareVersionChanged)
    Q_PROPERTY(QString releaseNotes READ getChangelog NOTIFY changelogChanged)
public:
    explicit MRemoteInterface(QObject *parent = nullptr);
    const static double CURRENT_VERSION;

signals:
    void validationNewVersionAvailable(QString versionMessage);

    void softwareVersionChanged();
    void changelogChanged();
public slots:
    static QString getDisplayVersion();

    void doUpdate();
    void setLocalSetting(QString key, QString value);
    QString getLocalSetting(QString key);
    void requestChangelog();

    QString getSoftwareVersion();
    QString getChangelog();
    void die();

private slots:
    void replyFinished(QNetworkReply *reply);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    enum RequestType {
        NONE,
        CHANGELOG
    };
    struct Request {
        Request(RequestType t, QNetworkRequest r) : type(t), request(r), reply(nullptr), numRedirects(0) {}
        Request() : type(RequestType::NONE), request(), reply(nullptr), numRedirects(0) {}
        RequestType type;
        QNetworkRequest request;
        QNetworkReply* reply;
        size_t numRedirects;
    };
    void handleChangelogResponse(QByteArray &data);
    void processLatestVersion(QJsonValue &version);
    void sendRequest(Request request);

    MSettings settings;
    QNetworkAccessManager networkManager;
    QTimer killTimer;
    QString avaliableVersion;
    QString changelog;
    QMutex queueMutex;
    std::vector<Request> pendingRequests;
    bool firstResponseProcessed = false;
};

#endif // MREMOTEINTERFACE_H
