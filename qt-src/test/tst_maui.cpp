#include <QtTest>
#include <QCoreApplication>

// add necessary includes here

#include "mremoteinterface.h"

class MAUI : public QObject
{
    Q_OBJECT

public:
    MAUI();
    ~MAUI();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_remoteAuth();
    void test_remoteChangelog();

private:
    QString orig_user;
    QString orig_pw;

    const int MAX_TIMEOUT = 5000;

};

MAUI::MAUI()
{

}

MAUI::~MAUI()
{

}

void MAUI::initTestCase()
{
    MRemoteInterface remote;
    orig_user = remote.getUsername();
    orig_pw = remote.getPassword();
}

void MAUI::cleanupTestCase()
{
    MRemoteInterface remote;
    remote.changeExistingCredentials(orig_user, orig_pw);
}

void MAUI::test_remoteAuth()
{
    MRemoteInterface remote;
    QCOMPARE(remote.getSoftwareVersion(), "Checking...");

    // we use stored credentials on this computer
    // if these are empty, run MAUI once and login to setup a user
    QVERIFY(!remote.getUsername().isEmpty());
    QVERIFY(!remote.getPassword().isEmpty());

    QSignalSpy validationFailed(&remote, SIGNAL(validationFailed(QString)));
    QSignalSpy validationNoConnection(&remote, SIGNAL(validationNoConnection()));
    QSignalSpy validationSuccess(&remote, SIGNAL(validationSuccess()));
    QSignalSpy validationAccountExpired(&remote, SIGNAL(validationAccountExpired()));
    QSignalSpy validationBadCredentials(&remote, SIGNAL(validationBadCredentials()));
    QSignalSpy validationNewVersionAvailable(&remote, SIGNAL(validationNewVersionAvailable(QString)));
    QSignalSpy noExistingCredentials(&remote, SIGNAL(noExistingCredentials()));
    QSignalSpy multipleSessionsDetected(&remote, SIGNAL(multipleSessionsDetected()));
    QSignalSpy sessionFinished(&remote, SIGNAL(sessionFinished()));

    QVERIFY(validationFailed.isValid());
    QVERIFY(validationNoConnection.isValid());
    QVERIFY(validationSuccess.isValid());
    QVERIFY(validationAccountExpired.isValid());
    QVERIFY(validationBadCredentials.isValid());
    QVERIFY(validationNewVersionAvailable.isValid());
    QVERIFY(noExistingCredentials.isValid());
    QVERIFY(multipleSessionsDetected.isValid());
    QVERIFY(sessionFinished.isValid());

    // initial validation with stored credentials
    remote.validateWithExistingCredentials();
    QTest::qWaitFor([&]() {
        return validationSuccess.count() > 0 || multipleSessionsDetected.count() > 0;
    }, MAX_TIMEOUT);

    QVERIFY(validationSuccess.count() == 1 || multipleSessionsDetected.count() == 1);
    qDebug() << "passing test with login status: " << (validationSuccess.count() > 0 ? "success" : "multiple accounts detected");
    QVERIFY(validationFailed.count() == 0);
    QVERIFY(validationNoConnection.count() == 0);
    QVERIFY(validationAccountExpired.count() == 0);
    QVERIFY(validationBadCredentials.count() == 0);
    QVERIFY(validationNewVersionAvailable.count() == 0);
    QVERIFY(noExistingCredentials.count() == 0);
    QVERIFY(sessionFinished.count() == 0);

    // this should now be updated from the response
    QCOMPARE(remote.getSoftwareVersion(), QString::number(MRemoteInterface::CURRENT_VERSION));

    int detections = multipleSessionsDetected.count();

    // trigger multiple sessions
    remote.validateWithExistingCredentials();
    QTest::qWaitFor([&]() {
        return multipleSessionsDetected.count() == (detections + 1);
    }, MAX_TIMEOUT);

    QVERIFY(validationFailed.count() == 0);
    QVERIFY(validationNoConnection.count() == 0);
    QVERIFY(validationAccountExpired.count() == 0);
    QVERIFY(validationBadCredentials.count() == 0);
    QVERIFY(validationNewVersionAvailable.count() == 0);
    QVERIFY(noExistingCredentials.count() == 0);
    QVERIFY(sessionFinished.count() == 0);

    // try to login with bad credentials
    remote.validateRequest("some_user@test.com", "invalid_test_password");
    QTest::qWaitFor([&]() {
        return validationBadCredentials.count() == 1;
    }, MAX_TIMEOUT);

    QVERIFY(validationFailed.count() == 0);
    QVERIFY(validationNoConnection.count() == 0);
    QVERIFY(validationAccountExpired.count() == 0);
    QVERIFY(validationBadCredentials.count() == 1);
    QVERIFY(validationNewVersionAvailable.count() == 0);
    QVERIFY(noExistingCredentials.count() == 0);
    QVERIFY(sessionFinished.count() == 0);
}

// FIXME: this test sometimes fails when running against a "runserver" invoked test environment on localhost
void MAUI::test_remoteChangelog()
{
    MRemoteInterface remote;
    QString defaultChangelogText = "Fetching changes...";
    QCOMPARE(remote.getChangelog(), defaultChangelogText);

    QSignalSpy changelogUpdated(&remote, SIGNAL(changelogChanged()));
    QVERIFY(changelogUpdated.isValid());
    remote.requestChangelog();
    QTest::qWaitFor([&]() {
        return changelogUpdated.count() > 0;
    }, MAX_TIMEOUT);

    QVERIFY(changelogUpdated.count() == 1);
    QString changelog = remote.getChangelog();
    QVERIFY(changelog != defaultChangelogText);
    QVERIFY(changelog.length() > 100);
    QVERIFY(changelog.contains("version " + QString::number(MRemoteInterface::CURRENT_VERSION), Qt::CaseSensitivity::CaseInsensitive));
}

QTEST_MAIN(MAUI)

#include "tst_maui.moc"
