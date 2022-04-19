#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QFileDialog>
#include <QMenu>

//test
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();

    setWindowTitle("木头人播放系统");
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

    // 自定义菜单
    connect(ui->widgetBase, &QWidget::customContextMenuRequested, this, &MainWindow::slot_custom_context_menu_requested);
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
        mLastMousePosition = event->globalPosition();
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
    const QPointF position = pos() + event->globalPosition() - mLastMousePosition; //the position of mainfrmae + (current_mouse_position - last_mouse_position)
    move(position.x(), position.y());
    mLastMousePosition = event->globalPosition();
}

void MainWindow::slot_btnClose_clicked()
{
    exit(0);
}

void MainWindow::slot_open_file()
{
    QStringList fileName = QFileDialog::getOpenFileNames(nullptr, tr("选择视频文件"), "", tr("视频文件 (*.mp4 *.mkv *.avi *.mov *.flv *.wmv *.mpg)"));
    if (fileName.isEmpty()) return;



    if (fileName.size() == 0) return;

    ui->widgetPlayer->setVolume(0.36);
    ui->widgetPlayer->play(fileName.at(0));
}

void MainWindow::slot_custom_context_menu_requested(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu menu(this);
    QAction actionOpen("打开文件");
    connect(&actionOpen, &QAction::triggered, this, &MainWindow::slot_open_file);
    menu.addAction(&actionOpen);

    QAction actionClose("关闭程序");
    connect(&actionClose, &QAction::triggered, this, []{ exit(0); });
    menu.addAction(&actionClose);

    menu.exec(QCursor::pos());
}

