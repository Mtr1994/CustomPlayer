#ifndef WIDGETMEDIACONTROL_H
#define WIDGETMEDIACONTROL_H

#include <QWidget>

namespace Ui {
class WidgetMediaControl;
}

class WidgetMediaControl : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetMediaControl(QWidget *parent = nullptr);
    ~WidgetMediaControl();

    void init();

    float getVolume();

signals:
    void sgl_video_volume_changed(float volume);
    void sgl_seek_video_pts(int64_t pts);

    void sgl_trigger_action_play();
    void sgl_trigger_action_pause();

    void sgl_trigger_action_previous_frame();
    void sgl_trigger_action_next_frame();

    void sgl_trigger_action_speed_change(float speed);

public slots:
    void slot_video_duration_changed(int64_t duration);
    void slot_video_position_change(double process);
    void slot_duration_slider_moved(int64_t pts);

private slots:
    void slot_button_play_clicked();
    void slot_button_speed_clicked();

    void slot_current_speed_change(int index, const QString &text);

private:
    void updateTime();

private:
    Ui::WidgetMediaControl *ui;

    // seek frame flag
    bool mIsSeekFrame = false;

    uint64_t mVideoProcess = 0;
    uint64_t mVideoDuration = 0;

    double mCurrentSpeed = 1;
};

#endif // WIDGETMEDIACONTROL_H
