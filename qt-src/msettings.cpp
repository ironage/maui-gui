#include "msettings.h"

#include <QDateTime>
#include <QDebug>
#include <QUuid>

MSettings::MSettings(): settings("MAUI", "GUI")
{
}

QString MSettings::getVersionEndpoint()
{
    // changed from v4.7 "baseUrl" to baseUrlSecure to use https
    // changed from v4.8 "baseUrlSecure" to "baseUrlSecureApp" from https://www.hedgehogmedical.com/users/ to https://app.hedgehogmedical.com/
    // changed from v5.1 "baseUrlSecureApp" to "versionEndpoint" from https://app.hedgehogmedical.com/ to https://api.github.com/repos/ironage/maui-gui/releases/latest
    QString stored = settings.value("versionEndpoint", "").toString();
    if (stored.isEmpty()) {
        stored = "https://api.github.com/repos/ironage/maui-gui/releases/latest";
        settings.setValue("versionEndpoint", stored);
    }
    //stored = "http://localhost:8000/"; // FIXME: local dev
    //stored = "http://hedgehogmedical.com/users/"; // FIXME: local dev
    return stored;
}

void MSettings::setVersionEndpoint(QString newUrl)
{
    settings.setValue("versionEndpoint", newUrl);
}

void MSettings::setRaw(QString key, QString value)
{
    settings.setValue(key, value);
}

QString MSettings::getRaw(QString key, QString defaultValue)
{
    return settings.value(key, defaultValue).toString();
}
