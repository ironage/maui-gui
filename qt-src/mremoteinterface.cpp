#include "mremoteinterface.h"

MRemoteInterface::MRemoteInterface(QObject *parent) : QObject(parent)
{

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

void MRemoteInterface::validate(QString username, QString password)
{
    emit validationSuccess();
}

void MRemoteInterface::validateRequest(QString username, QString password)
{
    settings.setUsername(username);
    settings.setPassword(password);
    validate(username, password);
}
