#ifndef MREMOTEINTERFACE_H
#define MREMOTEINTERFACE_H

#include "msettings.h"

#include <QObject>

class MRemoteInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString username READ getUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ getPassword NOTIFY passwordChanged)
public:
    explicit MRemoteInterface(QObject *parent = 0);

signals:
    void validationFailed();
    void validationNoConnection();
    void validationSuccess();
    void validationAccountExpired();
    void validationBadCredentials();
    void validationNewVersionAvailable();
    void noExistingCredentials();

    void usernameChanged();
    void passwordChanged();
public slots:
    void validateRequest(QString username, QString password);
    void validateWithExistingCredentials();

    QString getUsername();
    QString getPassword();

private:
    void validate(QString username, QString password);
    MSettings settings;
};

#endif // MREMOTEINTERFACE_H
