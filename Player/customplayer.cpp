#include "customplayer.h"

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QGridLayout>
#include <QImage>
#include <QPainter>
#include <QVideoSink>
#include <QAudioSink>
#include <QAudioOutput>

// test
#include <QDebug>

CustomPlayer::CustomPlayer(QWidget *parent) :
    QOpenGLWidget(parent)
{
    init();
}

CustomPlayer::~CustomPlayer()
{
    disconnect(mVideoSink, &QVideoSink::videoFrameChanged, this, &CustomPlayer::slot_play_current_frame);

    mMediaPlayer->stop();
    mMediaPlayer->deleteLater();
}

void CustomPlayer::init()
{
    mMediaPlayer = new QMediaPlayer(this);
    connect(mMediaPlayer, &QMediaPlayer::durationChanged, this, &CustomPlayer::slot_duration_changed);

    mVideoSink = new QVideoSink;
    connect(mVideoSink, &QVideoSink::videoFrameChanged, this, &CustomPlayer::slot_play_current_frame);
    mMediaPlayer->setVideoSink(mVideoSink);

    mAudioOutput = new QAudioOutput(this);
    mAudioOutput->setVolume(0.36);
    mMediaPlayer->setAudioOutput(mAudioOutput);
}

void CustomPlayer::play(const QString &path)
{
    mMediaPlayer->setSource(path);
    mMediaPlayer->play();
}

void CustomPlayer::start()
{
    mMediaPlayer->play();
}

void CustomPlayer::pause()
{
    mMediaPlayer->pause();
}

void CustomPlayer::stop()
{
    if (nullptr == mMediaPlayer) return;
    if (mMediaPlayer->mediaStatus() == QMediaPlayer::NoMedia) return;

    mMediaPlayer->stop();
}

void CustomPlayer::seek(int64_t seconds)
{
     mMediaPlayer->setPosition(seconds * 1000);
}

void CustomPlayer::setVolume(double volume)
{
    mAudioOutput->setVolume(volume);
}

void CustomPlayer::previous()
{
    mMediaPlayer->setPosition(mMediaPlayer->position() - mJumpSteps);
}

void CustomPlayer::next()
{
    mMediaPlayer->setPosition(mMediaPlayer->position() + mJumpSteps);
}

void CustomPlayer::changeRate(float rate)
{
    mMediaPlayer->setPlaybackRate(rate);
}

void CustomPlayer::slot_play_current_frame(const QVideoFrame &frame)
{
    mCurrentImage = frame.toImage();

    double rate = (double)frame.height() / frame.width();
    double widthSize = this->width();
    double heightSize = this->width() * rate;

    if (this->height() < heightSize)
    {
        heightSize = this->height();
        widthSize = heightSize / rate;
    }

    mVideoWidth = widthSize;
    mVideoHeight = heightSize;

    update();

    emit sgl_video_position_change((double)frame.startTime() / 1000.0);
}

void CustomPlayer::paintEvent(QPaintEvent *event)
{
    QRect backgroundRect = this->rect();

    QPainter painter;
    painter.begin(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(1, 1, 0)));
    painter.drawRect(backgroundRect);

    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QRectF imageRect = QRectF((backgroundRect.width() - mVideoWidth) * 0.5, (backgroundRect.height() - mVideoHeight) * 0.5, mVideoWidth ,mVideoHeight);
    painter.drawImage(imageRect, mCurrentImage);
    painter.end();

    QWidget::paintEvent(event);
}

void CustomPlayer::slot_duration_changed(qint64 duration)
{
    emit sgl_video_duration_change(duration);
}
