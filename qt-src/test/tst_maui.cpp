#include <QtTest>
#include <QCoreApplication>

// add necessary includes here

#include "mremoteinterface.h"
#include "mcvplayer.h"

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
    void test_diameterVideo1();

private:
    QString orig_user;
    QString orig_pw;

    const int MAX_NETWORK_TIMEOUT = 5000;
    const int MAX_LIBRARY_LOAD_TIMEOUT = 10000;
    const int MAX_VIDEO_LOAD_TIMEOUT = 3000;

};

MAUI::MAUI()
{

}

MAUI::~MAUI()
{

}

void MAUI::initTestCase()
{
    {
        MSettings settings;
        QString serverUnderTest = settings.getBaseUrl();
        qDebug() << "server under test is located at: " << serverUnderTest;
    }

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
    QSKIP("skipping auth tests");
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
    }, MAX_NETWORK_TIMEOUT);

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
    }, MAX_NETWORK_TIMEOUT);

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
    }, MAX_NETWORK_TIMEOUT);

    QVERIFY(validationFailed.count() == 0);
    QVERIFY(validationNoConnection.count() == 0);
    QVERIFY(validationAccountExpired.count() == 0);
    QVERIFY(validationBadCredentials.count() == 1);
    QVERIFY(validationNewVersionAvailable.count() == 0);
    QVERIFY(noExistingCredentials.count() == 0);
    QVERIFY(sessionFinished.count() == 0);
}

// FIXME: this test sometimes fails when running against a "runserver" invoked test environment on localhost
// maybe check that django isn't caching or flushing something?
void MAUI::test_remoteChangelog()
{
    QSKIP("skipping changelog test");
    MRemoteInterface remote;
    QString defaultChangelogText = "Fetching changes...";
    QCOMPARE(remote.getChangelog(), defaultChangelogText);

    QSignalSpy changelogUpdated(&remote, SIGNAL(changelogChanged()));
    QVERIFY(changelogUpdated.isValid());
    remote.requestChangelog();
    QTest::qWaitFor([&]() {
        return changelogUpdated.count() > 0;
    }, MAX_NETWORK_TIMEOUT);

    QVERIFY(changelogUpdated.count() == 1);
    QString changelog = remote.getChangelog();
    QVERIFY(changelog != defaultChangelogText);
    QVERIFY(changelog.length() > 100);
    QVERIFY(changelog.contains("version " + QString::number(MRemoteInterface::CURRENT_VERSION), Qt::CaseSensitivity::CaseInsensitive));
}

void waitForEventLoop(int max_wait_ms = 1000)
{
    bool did_run = false;
    // timers are dispatched at the next event loop cycle
    QTimer::singleShot(0, [&]() {
        did_run = true;
    });
    QTest::qWaitFor([&]() {
        return did_run;
    }, max_wait_ms);
}

void verifyResults(QString baselinePath, QString resultsPath)
{
    QFile baselineFile(baselinePath);
    QFile resultsFile(resultsPath);
    qDebug() << "comparing baseline " << baselineFile.fileName() << " with results " << resultsFile.fileName();
    QVERIFY(baselineFile.exists());
    QVERIFY(resultsFile.exists());
    QVERIFY(baselineFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QVERIFY(resultsFile.open(QIODevice::ReadOnly | QIODevice::Text));

    QTextStream inBaseline(&baselineFile);
    QTextStream inResults(&resultsFile);
    size_t lineCount = 0;
    while (!inBaseline.atEnd()) {
        QVERIFY(!inResults.atEnd());
        QString baselineText = inBaseline.readLine();
        QString resultsText = inResults.readLine();

        if (lineCount > 4) {
            QCOMPARE(baselineText, resultsText);
        } else {
            // only compare after the first column
            QStringList baselineParts = baselineText.split(",");
            QStringList resultParts = resultsText.split(",");
            QVERIFY(baselineParts.size() > 1);
            QVERIFY(resultParts.size() > 1);
            if (lineCount == 3) {
                QCOMPARE(baselineParts[0], resultParts[0]); // unit conversion matches
            } else if (lineCount == 4) {
                QCOMPARE(resultParts[0], "MAUI version " + QString::number(MRemoteInterface::CURRENT_VERSION));
            }
            baselineParts.pop_front();
            resultParts.pop_front();
            QCOMPARE(baselineParts, resultParts);
        }
        ++lineCount;
    }
    QVERIFY(inResults.atEnd());
}

struct ResultsComparison
{
    QString inputPath;
    QString baselinePath;
    QSize videoSize;
    int numFrames;
    QString videoName;
    QString videoExtension;
    QRect roi;
    QRect diameterScale;
    double diameterConversion;
    QString diameterUnits;
    bool generateOutputVideo = false;
};

void MAUI::test_diameterVideo1()
{
    const QString videoDir = "../../videos/";
    const QString baselineDir = "../../videos/baseline/";
    const int MAX_PROCESSING_WAIT_TIME = 30000;
    const int MAX_OUTPUT_WRITE_TIME = 15000;
    const int MAX_COMPUTE_POINTS_TIME = 2000;

    MCVPlayer player;
    // wait for the matlab initialization
    QSignalSpy init(&player, SIGNAL(initFinished(MInitTask::InitStats)));
    QTest::qWaitFor([&]() {
        return init.count() > 0;
    }, MAX_LIBRARY_LOAD_TIMEOUT);
    QCOMPARE(init.count(), 1);
    QList<QVariant> initArgs = init.takeFirst(); // first signal
    QVERIFY(initArgs.at(0).toInt() == MInitTask::InitStats::SUCCESS);

    std::vector<ResultsComparison> comparisons;
    comparisons.push_back(
        {
            QFINDTESTDATA(videoDir + "sample01.avi"),
            QFINDTESTDATA(baselineDir + "sample01_diameter_baseline.csv"),
            QSize(1024, 768),
            223,
            "sample01",
            "avi",
            QRect(224, 136, 69, 301),
            QRect(852, 284, 0, 235),
            1.0,
            "cm",
            false
        });

    for (ResultsComparison& test : comparisons) {
        // load a video
        QVERIFY(!test.inputPath.isEmpty());
        QVERIFY(!test.baselinePath.isEmpty());
        QUrl inputUrl = QUrl::fromLocalFile(test.inputPath);
        QSignalSpy load(&player, SIGNAL(videoLoaded(bool, QUrl, QString, QString, QString)));
        player.addVideoFile(inputUrl.toString());
        QTest::qWaitFor([&]() {
            return load.count() > 0;
        }, MAX_VIDEO_LOAD_TIMEOUT);
        QCOMPARE(load.count(), 1);
        QList<QVariant> args = load.takeFirst(); // first signal
        QVERIFY(args.at(0).toBool() == true);
        QVERIFY(args.at(1).toUrl() == inputUrl);
        QCOMPARE(args.at(2).toString(), test.videoName);
        QCOMPARE(args.at(3).toString(), test.videoExtension);
        QVERIFY(args.at(4).toString().endsWith("videos"));
        QCOMPARE(player.getSize(), test.videoSize);
        QCOMPARE(player.getNumFrames(), test.numFrames);
        QCOMPARE(player.getCurFrame(), 0);
        QCOMPARE(player.getPlaybackState(), QMediaPlayer::PausedState);
        QCOMPARE(player.getSetupState(), CameraTask::SetupState::ALL);
        player.setSetupState(CameraTask::SetupState::NORMAL_ROI);
        waitForEventLoop();
        QCOMPARE(player.getSetupState(), CameraTask::SetupState::NORMAL_ROI);

        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        player.setOutputDir(dir.path());
        QCOMPARE(player.getOutputDir(), dir.path());
        QDir outputDir(dir.path());
        QVERIFY(outputDir.isEmpty());

        QSignalSpy pointsChanged(&player, SIGNAL(initPointsChanged()));
        player.setROI(test.roi);
        QCOMPARE(player.getROI(), test.roi);
        player.setProcessOutputVideo(test.generateOutputVideo);
        waitForEventLoop();
        QCOMPARE(player.getProcessOutputVideo(), test.generateOutputVideo);
        player.setDiameterScale(test.diameterScale);
        QCOMPARE(player.getDiameterScale(), test.diameterScale);
        player.setDiameterConversionUnits(test.diameterUnits);
        QCOMPARE(player.getDiameterConversionUnits(), test.diameterUnits);
        player.setDiameterConversion(test.diameterConversion);
        QCOMPARE(player.getDiameterConversion(), test.diameterConversion);
        pointsChanged.wait(MAX_COMPUTE_POINTS_TIME);
        QVERIFY(pointsChanged.count() > 0);
        QSignalSpy finished(&player, SIGNAL(videoFinished(CameraTask::ProcessingState)));
        QSignalSpy output(&player, SIGNAL(outputProgress(int)));

        player.play();
        finished.wait(MAX_PROCESSING_WAIT_TIME);
        QCOMPARE(finished.count(), 1);
        QList<QVariant> finishedArgs = finished.takeFirst(); // first signal
        QCOMPARE(finishedArgs.at(0).toInt(), CameraTask::ProcessingState::SUCCESS);

        output.wait(MAX_OUTPUT_WRITE_TIME);
        QTest::qWaitFor([&]() {
            return output.count() > 0 && output.last().at(0) == 100;
        }, MAX_OUTPUT_WRITE_TIME);

        QVERIFY(!outputDir.isEmpty());
        QStringList aviMatches = outputDir.entryList().filter(QRegExp(".*avi$"));
        QStringList csvMatches = outputDir.entryList().filter(QRegExp(".*csv$"));
        QCOMPARE(aviMatches.length(), (test.generateOutputVideo ? 1 : 0));
        QCOMPARE(csvMatches.length(), 1);
        verifyResults(test.baselinePath, outputDir.absoluteFilePath(csvMatches[0]));
    }
}

QTEST_MAIN(MAUI)

#include "tst_maui.moc"
