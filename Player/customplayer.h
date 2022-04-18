#ifndef CUSTOMPLAYER_H
#define CUSTOMPLAYER_H

#include <QWidget>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QList>
#include <QVideoFrame>
#include <QAbstractVideoBuffer>
#include <QOpenGLWidget>
#include <QImage>
#include <QPaintEvent>

// test
#include <QDebug>

class VideoFrameReceiver : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    VideoFrameReceiver() {}

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
    {
        if (type == QAbstractVideoBuffer::NoHandle)
        {
            return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_YUV420P << QVideoFrame::Format_RGB32;
        }

        return QList<QVideoFrame::PixelFormat>();
    };

    bool start(const QVideoSurfaceFormat &format) {Q_UNUSED(format);  return true; }

    void stop() {}

    bool present(const QVideoFrame &frame)
    {
        if (frame.isValid())
        {
            emit sgl_play_current_frame(frame);
        }
        return false;
    }

signals:
    void sgl_play_current_frame(const QVideoFrame &frame);
};

class QMediaPlayer;
class QVideoProbe;
class QAudioProbe;
class QVideoWidget;
class QMediaPlaylist;
class CustomPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit CustomPlayer(QWidget *parent = nullptr);
    ~CustomPlayer();

    void init();

    void play(const QString &path);

    void start();

    void pause();

    void stop();

    void seek(int64_t seconds);

    void setVolume(float volume);

    void previous();

    void next();

    void changeRate(float rate);

signals:
    void sgl_video_duration_change(int64_t duration);
    void sgl_video_position_change(double progress);

public slots:
    void slot_play_current_frame(const QVideoFrame &frame);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void slot_duration_changed(qint64 duration);

private:
    QMediaPlayer *mMediaPlayer = nullptr;
    QMediaPlaylist *mPlayList = nullptr;
    QVideoWidget *mVideoWidget = nullptr;

    QVideoProbe *mVideoProbe = nullptr;
    QAudioProbe *mAudioProbe = nullptr;

    uint32_t mJumpSteps = 5000;

    VideoFrameReceiver *mVideoFrameReceiver = nullptr;;

    // 当前视频帧
    QImage mCurrentImage;

    int mVideoWidth;
    int mVideoHeight;

    double mVideoSpeed = 1.0;
};

#endif // CUSTOMPLAYER_H
