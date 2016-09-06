#ifndef MREMOTEINTERFACE_H
#define MREMOTEINTERFACE_H

#include "msettings.h"

#include <QObject>
#include <QNetworkAccessManager>

class MRemoteInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString username READ getUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ getPassword NOTIFY passwordChanged)
public:
    explicit MRemoteInterface(QObject *parent = 0);

signals:
    void validationFailed(QString failureReason);
    void validationNoConnection();
    void validationSuccess();
    void validationAccountExpired();
    void validationBadCredentials();
    void validationNewVersionAvailable(QString versionMessage);
    void noExistingCredentials();
    void multipleSessionsDetected();

    void usernameChanged();
    void passwordChanged();
public slots:
    void validateRequest(QString username, QString password);
    void validateWithExistingCredentials();

    QString getUsername();
    QString getPassword();
private slots:
    void replyFinished(QNetworkReply *reply);


private:
    void validate(QString username, QString password);
    static QJsonArray encryptForServer(QString value, QByteArray key, QByteArray key2 = "");
    static QString decryptFromServer(QByteArray value, QByteArray key, QByteArray key2 = "");
    MSettings settings;
    QNetworkAccessManager networkManager;
    bool transactionActive;
};

#endif // MREMOTEINTERFACE_H
