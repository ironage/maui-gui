#include <QtTest>
#include <QCoreApplication>
#include <QScopeGuard>

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
    void test_remoteEndpoints();
    void test_diameterVideo1();

private:
    QString orig_user;
    QString orig_pw;

    const int MAX_NETWORK_TIMEOUT = 5000;
    const int MAX_LIBRARY_LOAD_TIMEOUT = 10000;
    const int MAX_VIDEO_LOAD_TIMEOUT = 3000;

    QString fetchChangelog(MRemoteInterface& remote);
};

MAUI::MAUI()
{

}

MAUI::~MAUI()
{

}

void waitForEventLoop(int max_wait_ms = 1000)
{
    bool did_run = false;
    // timers are dispatched at the next event loop cycle
    QTimer::singleShot(10, [&]() {
        did_run = true;
    });
    QTest::qWaitFor([&]() {
        return did_run;
    }, max_wait_ms);
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
    qDebug() << "testing with user account: " << orig_user;
}

void MAUI::cleanupTestCase()
{
    MRemoteInterface remote;
    remote.changeExistingCredentials(orig_user, orig_pw);
}

void MAUI::test_remoteAuth()
{
//    QSKIP("skipping remote auth");
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

QString MAUI::fetchChangelog(MRemoteInterface& remote)
{
    MSettings settings;
    qint64 begin = QDateTime::currentMSecsSinceEpoch();
    QSignalSpy changelogUpdated(&remote, SIGNAL(changelogChanged()));
    assert(changelogUpdated.isValid());
    remote.requestChangelog();
    int initialCount = changelogUpdated.count();
    QTest::qWaitFor([&]() {
        QCoreApplication::processEvents();
        return changelogUpdated.count() > initialCount;
    }, MAX_NETWORK_TIMEOUT);

    assert(changelogUpdated.count() == initialCount + 1);
    qDebug() << "fetched changelog from " << settings.getBaseUrl() << " in " << QDateTime::currentMSecsSinceEpoch() - begin << "ms";
    return remote.getChangelog();
}

void MAUI::test_remoteChangelog()
{
//    QSKIP("skipping changelog verifcation");

    MRemoteInterface remote;
    QString defaultChangelogText = "Fetching changes...";
    QCOMPARE(remote.getChangelog(), defaultChangelogText);

    QString changelog = fetchChangelog(remote);
    QVERIFY(changelog != defaultChangelogText);
    QVERIFY(changelog.length() > 100);
    QVERIFY(changelog.contains("version " + QString::number(MRemoteInterface::CURRENT_VERSION), Qt::CaseSensitivity::CaseInsensitive));
}

void MAUI::test_remoteEndpoints()
{
    MSettings settings;
    QString origBase = settings.getBaseUrl();
    QCOMPARE(origBase, "https://app.hedgehogmedical.com/");
    auto cleanup = qScopeGuard([&] { settings.setBaseUrl(origBase); });
    MRemoteInterface remote;
    QSignalSpy changelogUpdated(&remote, SIGNAL(changelogChanged()));
    QVERIFY(changelogUpdated.isValid());
    int initialReplies = changelogUpdated.count();

    QString changelog = fetchChangelog(remote);

    std::vector<QString> urlsToCheck = {
        "https://app.hedgehogmedical.com/redirected-changelog/", // a server test point
        "https://app.hedgehogmedical.com/",
        "http://app.hedgehogmedical.com/",    // should follow redirect to https version
        "http://hedgehogmedical.com/users/",  // test server backwards compatibility to transparently forward requests to app.hedgehogmedical.com
        "https://hedgehogmedical.com/users/" // same with https version
    };

    for (QString& url : urlsToCheck) {
        settings.setBaseUrl(url);
        QCOMPARE(url, settings.getBaseUrl());
        QString newChangelog = fetchChangelog(remote);
        QCOMPARE(changelog, newChangelog);
    }
    QCOMPARE(changelogUpdated.count(), initialReplies + 1 + int(urlsToCheck.size()));
}

void verifyResults(QString baselinePath, QString resultsPath, bool isCombinedResults, double tolerance)
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

        size_t linesUsedForMetadata = isCombinedResults ? 5 : 4;
        size_t versionLine = isCombinedResults ? 5 : 4;
        size_t allMatchingLine1 = isCombinedResults ? 3 : 3;
        size_t allMatchingLine2 = isCombinedResults ? 4 : 3;

        if (lineCount > linesUsedForMetadata) {
            QCOMPARE(baselineText, resultsText);
        } else {
            // only compare after the first column
            QStringList baselineParts = baselineText.split(",");
            QStringList resultParts = resultsText.split(",");
            QVERIFY(baselineParts.size() > 1);
            QVERIFY(resultParts.size() > 1);
            if (lineCount == allMatchingLine1 || lineCount == allMatchingLine2) {
                QCOMPARE(baselineParts[0], resultParts[0]); // unit conversion matches
            } else if (lineCount == versionLine) {
                QCOMPARE(resultParts[0], "MAUI version " + QString::number(MRemoteInterface::CURRENT_VERSION));
            }
            baselineParts.pop_front();
            resultParts.pop_front();
            if (baselineParts != resultParts) {
                bool didCompareWithTolerance = true;
                if (baselineParts.size() != resultParts.size()) {
                    qDebug() << "incompatible result sizes: ";
                    didCompareWithTolerance = false;
                }

                for (int i = 0; i < baselineParts.size(); ++i) {
                    QString baselinePart = baselineParts[i];
                    QString resultPart = resultParts[i];
                    if (baselinePart != resultPart) {
                        bool canConvert = false;
                        double baselineDouble = baselinePart.toDouble(&canConvert);
                        if (!canConvert) {
                            didCompareWithTolerance = false;
                            break;
                        }
                        double resultDouble = resultPart.toDouble(&canConvert);
                        if (!canConvert) {
                            didCompareWithTolerance = false;
                            break;
                        }
                        QVERIFY(resultDouble - tolerance <= baselineDouble && resultDouble + tolerance >= baselineDouble);
                    }
                }
                if (!didCompareWithTolerance) {
                    qDebug() << "verify fails on line " << lineCount << " baseline: \n" << baselineParts
                                 << "\n results: \n" << resultParts;
                    QCOMPARE(baselineParts, resultParts);
                }
            }
        }
        ++lineCount;
    }
    QVERIFY(inResults.atEnd());
    qDebug() << "verified " << lineCount << " lines match the baseline exactly";
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
    int startFrame = -1;
    int endFrame = -1;
    QRect velocityROI = QRect();
    QRect velocityScale = QRect();
    double velocityConversion = 1;
    QString velocityUnits = "cm/s";
    double resultsTolerance = 0;
};

void MAUI::test_diameterVideo1()
{
    QSKIP("skipping compute validation");

    const QString videoDir = "../../videos/";
    const QString imageDir = "../../videos/images/";
    const QString baselineDir = "../../videos/baseline/";
    const int MAX_PROCESSING_WAIT_TIME = 30000;
    const int MAX_OUTPUT_WRITE_TIME = 15000;
    const int MAX_COMPUTE_POINTS_TIME = 5000;

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

    std::vector<QString> image_type_copies = {"bmp", "jpg", "png", "tiff"};
    for (auto& type : image_type_copies) {
        // these images are converted from the original bmp and should give the same results
        ResultsComparison input = {
            QFINDTESTDATA(imageDir + "11.32.19 hrs __[0568752]." + type),
            QFINDTESTDATA(baselineDir + "11.32.19 hrs __[0568752]_diameter_baseline.csv"),
            QSize(640, 453),
            1,
            "11.32.19 hrs __[0568752]",
            type,
            QRect(151, 104, 96, 155),
            QRect(654, 248, 0, 124),
            3.1,
            "mm",
            false,
            };
        if (type == "jpg") {
            input.resultsTolerance = 0.1; // we're going to allow a bit of play becasue of the lower resolution
        }
        comparisons.push_back(input);
    }

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
    comparisons.push_back(
        {
            QFINDTESTDATA(videoDir + "sample02.avi"),
            QFINDTESTDATA(baselineDir + "sample02_diameter_baseline.csv"),
            QSize(1024, 768),
            223,
            "sample02",
            "avi",
            QRect(506, 178, 98, 263),
            QRect(833, 239, 0, 188),
            1.0,
            "cm",
            false,
            53,
            119
        });

    comparisons.push_back(
        {
            QFINDTESTDATA(videoDir + "velocityType1.AVI"),
            QFINDTESTDATA(baselineDir + "velocityType1_velocity_baseline.csv"),
            QSize(1024, 768),
            303,
            "velocityType1",
            "AVI",
            QRect(),
            QRect(),
            1.0,
            "cm",
            false,
            -1,
            -1,
            QRect(159, 363, 593, 145),
            QRect(925, 370, 0, 94),
            50.0,
            "cm/s"
        });
    comparisons.push_back(
        {
            QFINDTESTDATA(videoDir + "velocityType2.avi"),
            QFINDTESTDATA(baselineDir + "velocityType2_combined.csv"),
            QSize(1280, 1024),
            76,
            "velocityType2",
            "avi",
            QRect(816, 184, 44, 104),
            QRect(1351, 27, 0, 270),
            4,
            "cm",
            false,
            -1,
            -1,
            QRect(560, 465, 688, 112),
            QRect(1415, 345, 0, 70),
            100,
            "cm/s"
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
        QString path = args.at(4).toString();
        QVERIFY(path.endsWith("videos") || path.endsWith("images"));
        QCOMPARE(player.getSize(), test.videoSize);
        QCOMPARE(player.getNumFrames(), test.numFrames);
        QCOMPARE(player.getCurFrame(), 0);
        QCOMPARE(player.getPlaybackState(), QMediaPlayer::PausedState);

        bool computeDiameter = !test.roi.isNull();
        bool computeVelocity = !test.velocityROI.isNull();
        CameraTask::SetupState stateToSet = CameraTask::SetupState::NONE;
        if (computeDiameter) {
            stateToSet = CameraTask::SetupState(stateToSet | CameraTask::SetupState::NORMAL_ROI);
        }
        if (computeVelocity) {
            stateToSet = CameraTask::SetupState(stateToSet | CameraTask::SetupState::VELOCITY_ROI);
        }
        player.setSetupState(stateToSet);
        waitForEventLoop();
        QCOMPARE(player.getSetupState(), stateToSet);

        QTemporaryDir dir;
        //QDir dir(QDir::homePath() + "/maui-test-files/");
        QVERIFY(dir.isValid());
        player.setOutputDir(dir.path());
        QCOMPARE(player.getOutputDir(), dir.path());
        QDir outputDir(dir.path());
        QVERIFY(outputDir.isEmpty());

        QSignalSpy pointsChanged(&player, SIGNAL(initPointsChanged()));
        waitForEventLoop();
        if (test.startFrame >= 0) {
            player.setStartFrame(test.startFrame);
            player.seek(test.startFrame);
        }
        if (test.endFrame >= 0) {
            player.setEndFrame(test.endFrame);
        }
        if (computeDiameter) {
            player.setROI(test.roi);
            QCOMPARE(player.getROI(), test.roi);
            player.setRecomputeROIMode(true);
            QVERIFY(player.getRecomputeROIMode());
            player.setDiameterScale(test.diameterScale);
            QCOMPARE(player.getDiameterScale(), test.diameterScale);
            player.setDiameterConversionUnits(test.diameterUnits);
            QCOMPARE(player.getDiameterConversionUnits(), test.diameterUnits);
            player.setDiameterConversion(test.diameterConversion);
            QCOMPARE(player.getDiameterConversion(), test.diameterConversion);
            pointsChanged.wait(MAX_COMPUTE_POINTS_TIME);
            QVERIFY(pointsChanged.count() > 0);
        }
        if (computeVelocity) {
            player.setVelocityROI(test.velocityROI);
            QCOMPARE(player.getVelocityROI(), test.velocityROI);
            player.setVelocityScaleVertical(test.velocityScale);
            QCOMPARE(player.getVelocityScaleVertical(), test.velocityScale);
            player.setVelocityConversion(test.velocityConversion);
            QCOMPARE(player.getVelocityConversion(), test.velocityConversion);
            player.setVelocityConversionUnits(test.velocityUnits);
            QCOMPARE(player.getVelocityConversionUnits(), test.velocityUnits);
        }
        player.setProcessOutputVideo(test.generateOutputVideo);
        QTest::qWaitFor([&]() {
            return player.getProcessOutputVideo() == test.generateOutputVideo;
        }, 1000);
        QCOMPARE(player.getProcessOutputVideo(), test.generateOutputVideo);
        QSignalSpy finished(&player, SIGNAL(videoFinished(CameraTask::ProcessingState)));
        QSignalSpy output(&player, SIGNAL(outputProgress(int)));

        player.play();
        finished.wait(MAX_PROCESSING_WAIT_TIME);
        QCOMPARE(finished.count(), 1);
        QList<QVariant> finishedArgs = finished.takeFirst(); // first signal
        QCOMPARE(finishedArgs.at(0).toInt(), CameraTask::ProcessingState::SUCCESS);
        if (test.generateOutputVideo) {
            QTest::qWaitFor([&]() {
                if (output.count() > 0) {
                    qDebug() << "output result: " << output.last().at(0).toInt();
                }
                return output.count() > 0 && output.last().at(0).toInt() == 100;
            }, MAX_OUTPUT_WRITE_TIME);
            QVERIFY(output.count() > 1);
            QCOMPARE(output.last().at(0).toInt(), 100);
        }

        QVERIFY(!outputDir.isEmpty());
        QStringList aviMatches = outputDir.entryList().filter(QRegExp(".*avi$"));
        QStringList csvMatches = outputDir.entryList().filter(QRegExp(".*csv$"));
        QCOMPARE(aviMatches.length(), (test.generateOutputVideo ? 1 : 0));
        QVERIFY(csvMatches.length() >= 1); // combined outputs 3 csv files
        verifyResults(test.baselinePath, outputDir.absoluteFilePath(csvMatches[0]), computeDiameter && computeVelocity, test.resultsTolerance);
        player.removeVideoFile(inputUrl.toString());
    }
}

QTEST_MAIN(MAUI)

#include "tst_maui.moc"
