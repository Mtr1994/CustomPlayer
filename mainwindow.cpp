#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QFileDialog>

//test
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();

    setWindowTitle("中国科学院深海科学与工程研究所-潜器视频播放系统");
    setWindowFlags(Qt::CustomizeWindowHint);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QScreen *screen = QGuiApplication::screens().at(0);
    float width = 1024;
    float height = 720;
    if (nullptr != screen)
    {
        QRect rect = screen->availableGeometry();
        width = rect.width() * 0.64 < 1024 ? 1024 : rect.width() * 0.64;
        height = rect.height() * 0.64 < 720 ? 720 : rect.height() * 0.64;
    }

    resize(width, height);

    ui->lbTitle->setText("木头人视频播放器");

    ui->btnMin->setID(ButtonDesigned::Button_Min);
    ui->btnMax->setID(ButtonDesigned::Button_Max);
    ui->btnNormal->setID(ButtonDesigned::Button_Normal);
    ui->btnClose->setID(ButtonDesigned::Button_Close);

    ui->btnNormal->setVisible(false);
    connect(ui->btnClose, &QPushButton::clicked, this, &MainWindow::slot_btnClose_clicked);
    connect(ui->btnMin, &QPushButton::clicked, this, [this]{showMinimized();});
    connect(ui->btnMax, &QPushButton::clicked, this, [this]{ui->btnMax->setVisible(false); ui->btnNormal->setVisible(true); showMaximized();});
    connect(ui->btnNormal, &QPushButton::clicked, this, [this]{ui->btnMax->setVisible(true); ui->btnNormal->setVisible(false); showNormal();});

    connect(ui->widgetPlayer, &CustomPlayer::sgl_video_duration_change, ui->widgetMediaControl, &WidgetMediaControl::slot_video_duration_changed);
    connect(ui->widgetPlayer, &CustomPlayer::sgl_video_position_change, ui->widgetMediaControl, &WidgetMediaControl::slot_video_position_change);

    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_video_volume_changed, this, &MainWindow::slot_video_volume_changed);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_seek_video_pts, this, &MainWindow::slot_seek_video_pts);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_trigger_action_play, this, &MainWindow::slot_trigger_action_play);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_trigger_action_pause, this, &MainWindow::slot_trigger_action_pause);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_trigger_action_previous_frame, this, &MainWindow::slot_trigger_action_previous_frame);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_trigger_action_next_frame, this, &MainWindow::slot_trigger_action_next_frame);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_trigger_action_speed_change, this, &MainWindow::slot_trigger_action_speed_change);

    // 默认打开视频文件
    ui->widgetPlayer->play("./Videos/trailer.mp4");
}

void MainWindow::slot_media_pause()
{
    ui->widgetPlayer->pause();
}

void MainWindow::slot_media_start()
{
    ui->widgetPlayer->start();
}

void MainWindow::slot_seek_video_pts(int64_t pts)
{
    ui->widgetPlayer->seek(pts);
}

void MainWindow::slot_video_volume_changed(float volume)
{
    volume = volume / 100;
    ui->widgetPlayer->setVolume(volume);
}

void MainWindow::slot_stop_media()
{
    ui->widgetPlayer->stop();
    ui->widgetContent->hide();
}

void MainWindow::slot_trigger_action_play()
{
    ui->widgetPlayer->start();
}

void MainWindow::slot_trigger_action_pause()
{
    ui->widgetPlayer->pause();
}

void MainWindow::slot_trigger_action_previous_frame()
{
    ui->widgetPlayer->previous();
}

void MainWindow::slot_trigger_action_next_frame()
{
    ui->widgetPlayer->next();
}

void MainWindow::slot_trigger_action_speed_change(float speed)
{
    ui->widgetPlayer->changeRate(speed);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->pos().x() > ui->widgetTitle->width() || event->pos().y() > ui->widgetTitle->height()) return;
    if (event->button() == Qt::LeftButton)
    {
        mLastMousePosition = event->globalPos();
        mMousePressed = true;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mMousePressed = false;
    event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!mMousePressed) return;
    if (!event->buttons().testFlag(Qt::LeftButton)) return;
    const QPointF position = pos() + event->globalPos() - mLastMousePosition; //the position of mainfrmae + (current_mouse_position - last_mouse_position)
    move(position.x(), position.y());
    mLastMousePosition = event->globalPos();
}

void MainWindow::slot_btnClose_clicked()
{
    exit(0);
}

void MainWindow::slot_open_file()
{
    QStringList fileName = QFileDialog::getOpenFileNames(nullptr, tr("选择视频文件"), "", tr("视频文件 (*.mp4 *.mkv *.avi *.mov *.flv *.wmv *.mpg)"));
    if (fileName.isEmpty()) return;

    ui->widgetPlayer->play(fileName.at(0));

    if (fileName.size() == 0) return;

    ui->widgetPlayer->setVolume(float(0.32));

    resizeEvent(nullptr);
}
