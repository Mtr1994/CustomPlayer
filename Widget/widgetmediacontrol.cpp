#include "widgetmediacontrol.h"
#include "ui_widgetmediacontrol.h"
#include "widgetpopupmenu.h"

#include <QtMath>

//test
#include <QDebug>

WidgetMediaControl::WidgetMediaControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetMediaControl)
{
    ui->setupUi(this);

    init();
}

WidgetMediaControl::~WidgetMediaControl()
{
    delete ui;
}

void WidgetMediaControl::init()
{
    connect(ui->btnPlay, &QPushButton::clicked, this, &WidgetMediaControl::slot_button_play_clicked);

    connect(ui->btnPreviousFrame, &QPushButton::clicked, this, &WidgetMediaControl::sgl_trigger_action_previous_frame);
    connect(ui->btnNextFrame, &QPushButton::clicked, this, &WidgetMediaControl::sgl_trigger_action_next_frame);
    connect(ui->btnSpeed, &QPushButton::clicked, this, &WidgetMediaControl::slot_button_speed_clicked);

    connect(ui->slider, &QSlider::sliderMoved, this, &WidgetMediaControl::slot_duration_slider_moved);
    connect(ui->sliderVolume, &QSlider::sliderMoved, this, &WidgetMediaControl::sgl_video_volume_changed);
    ui->sliderVolume->setMaximum(100);
    ui->sliderVolume->setValue(36);
}

float WidgetMediaControl::getVolume()
{
    return ui->sliderVolume->value() / 100.0;
}

void WidgetMediaControl::slot_video_duration_changed(int64_t duration)
{
    ui->slider->setMaximum(duration);
    mVideoDuration = duration / 1000;

    updateTime();

    ui->btnPlay->setChecked(true);
}

void WidgetMediaControl::slot_video_position_change(double process)
{
    ui->slider->setValue(process);

    // 更新时间
    mVideoProcess = process / 1000;
    updateTime();
}

void WidgetMediaControl::slot_duration_slider_moved(int64_t pts)
{
    emit sgl_seek_video_pts(pts / 1000);
}

void WidgetMediaControl::slot_button_play_clicked()
{
    if (ui->btnPlay->isChecked())
    {
        emit sgl_trigger_action_play();
    }
    else
    {
        emit sgl_trigger_action_pause();
    }
}

void WidgetMediaControl::slot_button_speed_clicked()
{
    WidgetPopupMenu *menu = new WidgetPopupMenu(ui->btnSpeed);
    QStringList list;

    list.append(QString("倍速 x 4"));
    list.append(QString("倍速 x 2"));
    list.append(QString("原始速度"));
    list.append(QString("缓速 x 2"));
    list.append(QString("缓速 x 4"));

    menu->setMenuList(list);
    connect(menu, &WidgetPopupMenu::sgl_menu_item_select, this, &WidgetMediaControl::slot_current_speed_change);
    menu->showMenu();
}

void WidgetMediaControl::slot_current_speed_change(int index, const QString &text)
{
    if (index < 0) return;

    float speed = qPow(2, (2 - index));
    if (speed == mCurrentSpeed) return;
    mCurrentSpeed = speed;
    ui->btnSpeed->setText(text);

    emit sgl_trigger_action_speed_change(mCurrentSpeed);
}

void WidgetMediaControl::updateTime()
{
    ui->lbDuration->setText(QString("%1:%2:%3").arg(mVideoProcess / 3600, 2, 10, QLatin1Char('0')).arg(mVideoProcess / 60, 2, 10, QLatin1Char('0')).arg(mVideoProcess % 60, 2, 10, QLatin1Char('0')));
    ui->lbDurationLeft->setText(QString("%1:%2:%3").arg(mVideoDuration / 3600, 2, 10, QLatin1Char('0')).arg(mVideoDuration / 60, 2, 10, QLatin1Char('0')).arg(mVideoDuration % 60, 2, 10, QLatin1Char('0')));
}
