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
    static QString getRandomString(int length);
    QString getBaseUrl();
    void setUsername(QString name);
    void setPassword(QString pw);
private:
    QString getEncryptedSetting(QString key, QByteArray encryptionKey, QString defaultValue = "");
    void setEncryptedSetting(QString key, QString value, QByteArray encryptionKey);
    void initSalt();
    void newSalt();
    QSettings settings;
    static QString salt;
};

#endif // MSETTINGS_H
