#ifndef MREMOTEINTERFACE_H
#define MREMOTEINTERFACE_H

#include "msettings.h"

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QTimer>
#include <QQueue>

struct Metric
{
    Metric(QString inEvent, QString inExtension, int inFrameIndex, int inDuration, QString inSource, int setupState)
        : event(inEvent)
        , extension(inExtension)
        , frameIndex(inFrameIndex)
        , duration(inDuration)
        , source(inSource)
        , created(QDateTime::currentDateTimeUtc())
        , process(processFromSetupState(setupState))
    {
    }
    QString event;
    QString extension;
    int frameIndex;
    int duration;
    QString source;
    QDateTime created;
    QString process;
private:
    QString processFromSetupState(int setupState);
};

class MRemoteInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString username READ getUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ getPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString softwareVersion READ getSoftwareVersion NOTIFY softwareVersionChanged)
    Q_PROPERTY(QString releaseNotes READ getChangelog NOTIFY changelogChanged)
public:
    explicit MRemoteInterface(QObject *parent = nullptr);
    const static double CURRENT_VERSION;

signals:
    void validationFailed(QString failureReason);
    void validationNoConnection();
    void validationSuccess();
    void validationAccountExpired();
    void validationBadCredentials();
    void validationNewVersionAvailable(QString versionMessage);
    void noExistingCredentials();
    void multipleSessionsDetected();
    void sessionFinished();

    void usernameChanged();
    void passwordChanged();
    void softwareVersionChanged();
    void changelogChanged();
public slots:
    static QString getDisplayVersion();
    void validateRequest(QString username, QString password);
    void changeExistingCredentials(QString username, QString password);
    void validateWithExistingCredentials();
    void videoStateChange(QString playbackState, QString readSrcExtension, int frameIndex, int duration, QString source, int setupState);

    void finishSession();
    void doUpdate();
    void setLocalSetting(QString key, QString value);
    QString getLocalSetting(QString key);
    void requestChangelog();

    QString getUsername();
    QString getPassword();
    QString getSoftwareVersion();
    QString getChangelog();
private slots:
    void replyFinished(QNetworkReply *reply);
    void die();
    void processNextRequest();

private:
    enum RequestType {
        NONE,
        VERIFY,
        VERSION,
        CHANGELOG
    };
    struct Request {
        Request(RequestType t, QNetworkRequest r, QJsonObject j) : type(t), request(r), postData(j) {}
        RequestType type;
        QNetworkRequest request;
        QJsonObject postData;
        bool active = false;
    };
    void validate(QString username, QString password, QString method);
    void handleVerifyResponse(QByteArray &data);
    void handleVersionResponse(QByteArray &data);
    void handleChangelogResponse(QByteArray &data);
    void processLatestVersion(QJsonValue &version);
    static QJsonArray encryptForServer(QString value, QByteArray key, QByteArray key2 = "");
    static QString decryptFromServer(QByteArray value, QByteArray key, QByteArray key2 = "");
    MSettings settings;
    QNetworkAccessManager networkManager;
    QTimer killTimer;
    QString avaliableVersion;
    QString changelog;
    QQueue<Request> requestQueue;
    QQueue<Metric> metrics;
    bool firstResponseProcessed = false;
};

#endif // MREMOTEINTERFACE_H
