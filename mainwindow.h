﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QShowEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init();

public slots:
    void slot_media_pause();
    void slot_media_start();
    void slot_seek_video_pts(int64_t pts);
    void slot_video_volume_changed(float volume);
    void slot_stop_media();

    void slot_trigger_action_play();
    void slot_trigger_action_pause();

    void slot_trigger_action_previous_frame();
    void slot_trigger_action_next_frame();

    void slot_trigger_action_speed_change(float speed);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private slots:
    void slot_btnClose_clicked();
    void slot_open_file();

private:
    Ui::MainWindow *ui;

    QPointF mLastMousePosition;
    bool mMousePressed = false;
};
#endif // MAINWINDOW_H
