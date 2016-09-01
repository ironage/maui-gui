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
    QString getEncryptedSetting(QString key, QByteArray encryptionKey, QString defaultValue = "");
    void setEncryptedSetting(QString key, QString value, QByteArray encryptionKey);
    QSettings settings;
};

#endif // MSETTINGS_H
