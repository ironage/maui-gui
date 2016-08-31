#include "mremoteinterface.h"

MRemoteInterface::MRemoteInterface(QObject *parent) : QObject(parent)
{

}

void MRemoteInterface::validateWithExistingCredentials()
{
    emit noExistingCredentials();
}

QString MRemoteInterface::getUsername()
{
    return "user1";
}

QString MRemoteInterface::getPassword()
{
    return "pw";
}

void MRemoteInterface::validateRequest(QString username, QString password)
{
    emit validationSuccess();
}
