#include "mcvplayer.h"

#include <QDebug>
#include <QSize>
#include <QUrl>
#include <QRgb>

MCVPlayer::MCVPlayer() : QObject(),
    m_format(QSize(500, 500), QVideoFrame::Format_ARGB32),
    m_surface(NULL)
{

}

void MCVPlayer::setVideoSurface(QAbstractVideoSurface *surface)
{
    qDebug() << "setVideoSurface called";
    if (m_surface != surface && m_surface && m_surface->isActive()) {
        m_surface->stop();
    }
    m_surface = surface;
    if (m_surface)
        m_surface->start(m_format);
}

void MCVPlayer::onNewVideoContentReceived(const QVideoFrame &frame)
{
    qDebug() << "Presenting: " << frame << " to " << m_surface;
    if (m_surface) {
        qDebug() << "before present: " << m_surface->error();
        bool success = m_surface->present(frame);
        qDebug() << "after present: " << m_surface->error() << "success: " << success;

    }
}

void MCVPlayer::setSourceFile(QString file)
{
    QUrl fileUrl(file);
    qDebug() << "source file set: " << fileUrl.path();;
    sourceFile = fileUrl.path();
//    bool success = cvSource.open("/C:/Users/James/Documents/Programming/maui/maui-gui/videos/sample02.AVI");
    bool success = cvSource.open(0);
    //m_format.setFrameSize(cvSource.get(3), cvSource.get(4));

    qDebug() << "opened? " << success;

    process();
}

void MCVPlayer::process() {

    if (!cvSource.isOpened()) {
        qDebug() << "OpenCV source is not opened, cannot process frame";
        return;
    }
    cv::Mat frame;
    cvSource >> frame; // get a new frame from camera
    //cv::cvtColor(frame, edges, COLOR_BGR2GRAY);
    //cv::GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
    //cv::Canny(edges, edges, 0, 30, 3);

    cv::resize(frame, frame, cv::Size(), 0.3, 0.3, cv::INTER_AREA);
    cv::cvtColor(frame, frame, CV_BGR2RGB);
    const QImage image(frame.data, frame.cols, frame.rows, frame.step,
                       QImage::Format_RGB888, &matDeleter, new cv::Mat(frame));

    QImage *img2 = new QImage(QString("C:/Users/James/Documents/Programming/maui/maui-gui/build-maui-gui-Desktop_Qt_5_6_0_MSVC2013_64bit-Debug/debug/t1.png"));
    img2->save("meaning.png");
    m_format.setFrameSize(img2->width(), img2->height());
    m_format.setScanLineDirection(QVideoSurfaceFormat::TopToBottom);
    //m_format.setYCbCrColorSpace(QVideoSurfaceFormat::);
    if (m_surface) {
        m_surface->stop();
        m_surface->start(m_format);
    }

    QVideoFrame aFrame(32 * m_format.frameWidth()  * m_format.frameHeight(),m_format.frameSize(), 32 * m_format.frameWidth(),m_format.pixelFormat());

    aFrame.map(QAbstractVideoBuffer::WriteOnly);

    uchar * pixels = aFrame.bits();
    pixels[0] = 5;
    // perform pixel manipulation here...

    aFrame.unmap();

    qDebug() << "image data: " << img2 << " format: " << m_format;
    Q_ASSERT(image.constBits() == frame.data);
    QVideoFrame::PixelFormat img_format = QVideoFrame::pixelFormatFromImageFormat(img2->format());

    QVideoFrame *qFrame = new QVideoFrame(*img2);
    QVideoFrame::PixelFormat actual_format = qFrame->pixelFormat();
    qDebug() << "img format: " << img_format << " actual format: " << actual_format;
    qDebug() << "color space: " << m_format.yCbCrColorSpace();
    qDebug() << "property names: " << m_format.propertyNames();
    curFrame = *img2;
    onNewVideoContentReceived(*qFrame);
    //cv::imshow("frame", frame);
    //cv::waitKey(300);
//    emit imageReady(image);
}
