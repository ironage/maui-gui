#include "msettings.h"

#include "qblowfish.h"

#include <QDateTime>
#include <QDebug>
#include <QUuid>

static QByteArray UN_KEY = "7dsie41105ZFSKq0tkI8LkBG36f82f9b";
static QByteArray PW_KEY = "FBF1YD9ZcLggHFpr846PT5CVymk6281P";
static QByteArray SALT_KEY = "j7AzZsWt2eme9mMm8i5hOvDIrOv0ABMJ";

MSettings::MSettings(): settings("MAUI", "GUI")
{
    initSalt();
}

QString MSettings::getUsername()
{
    return getEncryptedSetting("username", UN_KEY, "");
}

QString MSettings::getPassword()
{
    return getEncryptedSetting("password", PW_KEY, "");
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

void MSettings::initSalt()
{
    QByteArray storedSalt = settings.value("salt", "").toByteArray();
    if (storedSalt.isEmpty()) {
        newSalt();
    } else {
        QBlowfish bf(SALT_KEY);
        bf.setPaddingEnabled(true);
        QByteArray rawSalt = bf.decrypted(storedSalt);
        if (rawSalt.isEmpty()) {
            newSalt();
        } else {
            salt = QString::fromStdString(rawSalt.toStdString());
        }
    }
}

void MSettings::newSalt()
{
    QBlowfish bf(SALT_KEY);
    bf.setPaddingEnabled(true);
    qsrand(uint(QDateTime::currentMSecsSinceEpoch()));
    salt = QString::number(qrand());
    qDebug() << "salt: " << salt;
    QByteArray encryptedSalt = bf.encrypted(QByteArray::fromRawData(salt.toStdString().c_str(), salt.size()));
    settings.setValue("salt", encryptedSalt);
}

QString MSettings::getBaseUrl()
{
    QString stored = settings.value("baseUrl", "").toString();
    if (stored.isEmpty()) {
        stored = "http://www.hedgehogmedical.com/users/";
        settings.setValue("baseUrl", stored);
    }
    return stored;
}

void MSettings::setUsername(QString name)
{
    setEncryptedSetting("username", name, UN_KEY);
}

void MSettings::setPassword(QString pw)
{
    setEncryptedSetting("password", pw, PW_KEY);
}

QString MSettings::getEncryptedSetting(QString key, QByteArray encryptionKey, QString defaultValue)
{
    QByteArray rawValue = settings.value(key, defaultValue).toByteArray();
    if (rawValue != defaultValue && !rawValue.isEmpty() && !encryptionKey.isEmpty()) {
        QBlowfish bfs(QByteArray::fromRawData(salt.toStdString().c_str(), salt.size()));
        bfs.setPaddingEnabled(true);
        rawValue = bfs.decrypted(rawValue);
        QBlowfish bf(encryptionKey);
        bf.setPaddingEnabled(true);
        rawValue = bf.decrypted(rawValue);
    }
    return QString::fromStdString(rawValue.toStdString());
}

void MSettings::setEncryptedSetting(QString key, QString value, QByteArray encryptionKey)
{
    std::string stdString = value.toStdString();
    QByteArray storable = QByteArray::fromRawData(stdString.c_str(), value.size());
    if (!encryptionKey.isEmpty()) {
        QBlowfish bf(encryptionKey);
        bf.setPaddingEnabled(true);
        storable = bf.encrypted(storable);
        QBlowfish bfs(QByteArray::fromRawData(salt.toStdString().c_str(), salt.size()));
        bfs.setPaddingEnabled(true);
        storable = bfs.encrypted(storable);
    }
    settings.setValue(key, storable);
}
