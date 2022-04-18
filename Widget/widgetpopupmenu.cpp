#include "widgetpopupmenu.h"
#include "Public/shadowfactory.h"

#include <QPainter>
#include <QMenu>
#include <QGridLayout>
#include <QStandardItem>
#include <QListView>
#include <QApplication>
#include <QPoint>
#include <QPainterPath>
#include <QtMath>

// test
#include <QDebug>
#include <QLabel>

int8_t WidgetPopupMenu::mGlobalMenuCount = 0;

WidgetPopupMenu::WidgetPopupMenu(QWidget *target)
    : QWidget{target}, mTargetWidgetPtr(target)
{
    init();
}

WidgetPopupMenu::~WidgetPopupMenu()
{
    delete mMenuModel;

    delete mListView;
}

void WidgetPopupMenu::init()
{
    QFont font("Microsoft YaHei", 9);
    QFontMetrics metrics(font);
    QRect fontRect = metrics.boundingRect("H");
    mContentMargin = qCeil(fontRect.height() * 0.24);

    mMenuModel = new QStandardItemModel(this);

    mListView = new QListView(this);
    mListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mListView->setEditTriggers(QListView::NoEditTriggers);
    mListView->setAutoScroll(false);
    mListView->setModel(mMenuModel);

    connect(mListView, &QListView::clicked, this, &WidgetPopupMenu::slot_menu_item_click);

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(mContentMargin, mContentMargin, mContentMargin, mContentMargin);
    layout->addWidget(mListView);
    setLayout(layout);

    setStyle();

    setAttribute(Qt::WA_TranslucentBackground);

    // 添加事件过滤器
    qApp->installEventFilter(this);
}

void WidgetPopupMenu::setMenuList(const QStringList &list)
{
    QFont font("Microsoft YaHei", 9);
    QFontMetrics metrics(font);
    for (auto &menu : list)
    {
        mMaxWidth = qMax(mMaxWidth, metrics.boundingRect(menu).width());
    }

    resize(mMaxWidth * 2.4 + mContentMargin * 2, metrics.height() * 2 * list.count() + mContentMargin * 2 + mContentMargin * 2 + 1);

    mMenuModel->clear();
    for (int i = 0; i < list.count(); i++)
    {
        QStandardItem *item = new QStandardItem(list.at(i));
        item->setTextAlignment(Qt::AlignCenter);
        item->setSizeHint(QSize(this->width(), metrics.height() * 2));
        mMenuModel->appendRow(item);
    }
}

void WidgetPopupMenu::showMenu()
{
    if (nullptr == mTargetWidgetPtr) return closeMenu();
    if (nullptr == topLevelWidget()) return closeMenu();

    if (mGlobalMenuCount < 0)
    {
        mGlobalMenuCount = 1;
        return closeMenu();
    }

    int width = this->width();
    int height = this->height();

    int targetX = mTargetWidgetPtr->mapToGlobal(QPoint(0, 0)).x();
    int targetY = mTargetWidgetPtr->mapToGlobal(QPoint(0, 0)).y();
    int targetWidth = mTargetWidgetPtr->width();
    int targetHeight = mTargetWidgetPtr->height();

    int realX = topLevelWidget()->mapFromGlobal(QPoint(targetX, targetY)).x();
    int realY = topLevelWidget()->mapFromGlobal(QPoint(targetX, targetY)).y();

    if (mMenuPosition == P_Top_Center)
    {
        setGeometry(realX + targetWidth * 0.5 - width * 0.5, realY - height - mContentMargin, width, height);
    }
    else if (mMenuPosition == P_Bottom_Center)
    {
        setGeometry(realX + targetWidth * 0.5 - width * 0.5, realY + targetHeight + mContentMargin, width, height);
    }

    // 全局统计
    mGlobalMenuCount++;

    setParent(topLevelWidget());
    setVisible(true);
    setFocus();
}

void WidgetPopupMenu::setColor(const QColor &color)
{
    mColor = color;
    setStyle();
}

void WidgetPopupMenu::setSelectColor(const QColor &color)
{
    mSelectColor = color;
    setStyle();
}

void WidgetPopupMenu::setBackgroundColor(const QColor &color)
{
    mBackgroundColor = color;
    setStyle();
}

void WidgetPopupMenu::setMenuPosition(Menu_Position position)
{
    mMenuPosition = position;
}

void WidgetPopupMenu::paintEvent(QPaintEvent *event)
{
    int width = this->width();
    int height = this->height();

    QRect rect = this->rect();
    QPainter painter(this);

    QSize targetSize = QSize(width - mContentMargin * 2, height - mContentMargin * 2);
    QRect shadowBaseRect = QRect(QPoint(0, 0), targetSize);
    shadowBaseRect.moveCenter(rect.center());
    if(mContentMargin >= 0)
    {
        QImage image = ShadowFactory::createShadow(targetSize, mContentMargin, QColor(224, 224, 224), QColor(254, 254, 254), this->devicePixelRatioF());
        QPoint diff(mContentMargin, mContentMargin);
        painter.drawImage(shadowBaseRect.topLeft() - diff, image);
    }

    QWidget::paintEvent(event);
}

bool WidgetPopupMenu::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type() == QEvent::ActivationChange)
    {
        if (QApplication::activeWindow() != this)
        {
            closeMenu();
        }
    }
    else if (ev->type() == QEvent::NonClientAreaMouseButtonPress)
    {
        closeMenu();
    }
    else if (ev->type() == QEvent::MouseButtonPress)
    {
        QPoint point = mapToGlobal(QPoint(0, 0));
        QRect rect(point, QSize(width(), height()));

        QPoint globalPoint = QCursor::pos();
        if (!rect.contains(globalPoint))
        {
            point = mTargetWidgetPtr->mapToGlobal(QPoint(0, 0));
            QRect rect(point, mTargetWidgetPtr->size());
            QPoint globalPoint = QCursor::pos();
            if (rect.contains(globalPoint))
            {
                mGlobalMenuCount = -1;
                closeMenu();
            }
            else
            {
                closeMenu();
            }
        }
    }
    return QObject::eventFilter(obj, ev);
}

void WidgetPopupMenu::slot_menu_item_click(const QModelIndex &index)
{
    if (!index.isValid()) return;

    emit sgl_menu_item_select(index.row(), mMenuModel->itemFromIndex(index)->text());

    closeMenu();
}

QString WidgetPopupMenu::parseColor(const QColor &color)
{
    return QString("rgba(%1, %2, %3, %4)").arg(QString::number(color.red()),
                                              QString::number(color.green()),
                                              QString::number(color.blue()),
                                               QString::number(color.alpha()));
}

void WidgetPopupMenu::setStyle()
{
    mListView->setStyleSheet("QListView"
                            "{"
                            "border: none;"
                            "background-color: transparent;"
                            "outline: none;" +
                            QString("color: %1;").arg(parseColor(mColor)) +
                            QString("padding: %1 0").arg(QString::number(mContentMargin)) +
                            "}"
                            "QListView::item:hover"
                            "{" +
                            QString("background-color: %1;").arg(parseColor(mSelectColor)) +
                            "border: none;" +
                            QString("color: %1;").arg(parseColor(mColor)) +
                            "}"
                            "QListView::item:selected"
                            "{"
                            "color: #505659;" +
                            QString("background-color: %1;").arg(parseColor(mSelectColor)) +
                            "border: none;"
                            "}"
                            "QListView::item"
                            "{"
                            "padding-left: 0;" +
                            QString("color: %1;").arg(parseColor(mColor)) +
                            "}"
                            );
}

void WidgetPopupMenu::closeMenu()
{
    // 删除事件过滤器
    qApp->removeEventFilter(this);

    // 全局数量
    if (mGlobalMenuCount > 0) mGlobalMenuCount--;

    setVisible(false);
    deleteLater();
}
