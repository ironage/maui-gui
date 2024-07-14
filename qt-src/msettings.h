#ifndef MSETTINGS_H
#define MSETTINGS_H

#include <QSettings>

class MSettings
{
public:
    MSettings();
    QString getVersionEndpoint();
    void setVersionEndpoint(QString newUrl);
    void setRaw(QString key, QString value);
    QString getRaw(QString key, QString defaultValue = "");
private:
    QSettings settings;
};

#endif // MSETTINGS_H
