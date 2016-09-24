#ifndef MREMOTEINTERFACE_H
#define MREMOTEINTERFACE_H

#include "msettings.h"


#include <QNetworkAccessManager>
#include <QObject>
#include <QTimer>

class MRemoteInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString username READ getUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ getPassword NOTIFY passwordChanged)
public:
    explicit MRemoteInterface(QObject *parent = 0);
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
public slots:
    static QString getDisplayVersion();
    void validateRequest(QString username, QString password);
    void validateWithExistingCredentials();
    void finishSession();
    void doUpdate();
    void setLocalSetting(QString key, QString value);
    QString getLocalSetting(QString key);

    QString getUsername();
    QString getPassword();
private slots:
    void replyFinished(QNetworkReply *reply);
    void die();

private:
    void validate(QString username, QString password, QString method);
    static QJsonArray encryptForServer(QString value, QByteArray key, QByteArray key2 = "");
    static QString decryptFromServer(QByteArray value, QByteArray key, QByteArray key2 = "");
    MSettings settings;
    QNetworkAccessManager networkManager;
    bool transactionActive;
    QTimer killTimer;
};

#endif // MREMOTEINTERFACE_H
