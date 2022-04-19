#ifndef CUSTOMPLAYER_H
#define CUSTOMPLAYER_H

#include <QOpenGLWidget>
#include <QVideoFrame>
#include <QImage>
#include <QPaintEvent>
#include <QOpenGLFunctions>
#include <QWidget>

// test
#include <QDebug>

class QMediaPlayer;
class QVideoSink;
class QAudioOutput;
class CustomPlayer : public QOpenGLWidget
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

    uint32_t mJumpSteps = 5000;

    QVideoSink *mVideoSink = nullptr;;
    QAudioOutput *mAudioOutput = nullptr;

    // 当前视频帧
    QImage mCurrentImage;

    int mVideoWidth;
    int mVideoHeight;
};

#endif // CUSTOMPLAYER_H
