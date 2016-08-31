#ifndef MSETTINGS_H
#define MSETTINGS_H

#include <QSettings>

class MSettings
{
public:
    MSettings();
    QString getUsername();
    QString getPassword();
    QString getUUID();
    QString getBaseUrl();
    void setUsername(QString name);
    void setPassword(QString pw);
private:
    QSettings settings;
};

#endif // MSETTINGS_H
