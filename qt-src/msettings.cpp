#include "msettings.h"
#include "qblowfish.h"

#include <QUuid>

MSettings::MSettings(): settings("MAUI", "GUI")
{
}

QString MSettings::getUsername()
{
    return settings.value("username", "").toString();
}

QString MSettings::getPassword()
{
    return settings.value("password", "").toString();
}

QString MSettings::getUUID()
{
    QString stored = settings.value("uuid", "").toString();
    if (stored.isEmpty()) {
        QUuid id = QUuid::createUuid();
        settings.setValue("uuid", id.toString());
        stored = id.toString();
    }
    return stored;
}

void MSettings::setUsername(QString name)
{
    settings.setValue("username", name);
}

void MSettings::setPassword(QString pw)
{
    settings.setValue("password", pw);
}
