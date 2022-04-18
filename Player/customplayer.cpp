#include "customplayer.h"

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaPlaylist>
#include <QGridLayout>
#include <QImage>
#include <QPainter>

// test
#include <QDebug>

CustomPlayer::CustomPlayer(QWidget *parent) :
    QWidget(parent)
{
    init();
}

CustomPlayer::~CustomPlayer()
{
    disconnect(mVideoFrameReceiver, &VideoFrameReceiver::sgl_play_current_frame, this, &CustomPlayer::slot_play_current_frame);
}

void CustomPlayer::init()
{
    mMediaPlayer = new QMediaPlayer(this);
    mMediaPlayer->setAudioRole(QAudio::VideoRole);
    mMediaPlayer->setVolume(36);

    mPlayList = new QMediaPlaylist;
    mMediaPlayer->setPlaylist(mPlayList);

    connect(mMediaPlayer, &QMediaPlayer::durationChanged, this, &CustomPlayer::slot_duration_changed);

    mVideoWidget = new QVideoWidget(this);
    mVideoFrameReceiver = new VideoFrameReceiver;
    connect(mVideoFrameReceiver, &VideoFrameReceiver::sgl_play_current_frame, this, &CustomPlayer::slot_play_current_frame);
    mMediaPlayer->setVideoOutput(mVideoFrameReceiver);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(mVideoWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
}

void CustomPlayer::play(const QString &path)
{
    mPlayList->addMedia(QMediaContent(path));
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

}

void CustomPlayer::seek(int64_t seconds)
{
     mMediaPlayer->setPosition(seconds * 1000);
}

void CustomPlayer::setVolume(float volume)
{
    mMediaPlayer->setVolume(volume * 100);
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
    mVideoSpeed = rate;
    mMediaPlayer->setPlaybackRate(rate);
}

void CustomPlayer::slot_play_current_frame(const QVideoFrame &frame)
{
    mCurrentImage = frame.image();

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

    emit sgl_video_position_change((double)frame.startTime() / 1000.0 * mVideoSpeed);
}

void CustomPlayer::paintEvent(QPaintEvent *event)
{
    QRect backgroundRect = this->rect();

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(0, 0, 0)));
    painter.drawRect(backgroundRect);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QRectF imageRect = QRectF((backgroundRect.width() - mVideoWidth) * 0.5, (backgroundRect.height() - mVideoHeight) * 0.5, mVideoWidth ,mVideoHeight);
    painter.drawImage(imageRect, mCurrentImage);

    QWidget::paintEvent(event);
}

void CustomPlayer::slot_duration_changed(qint64 duration)
{
    emit sgl_video_duration_change(duration);
}
