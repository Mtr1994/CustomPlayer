#ifndef WIDGETPOPUPMENU_H
#define WIDGETPOPUPMENU_H

#include <QWidget>
#include <QPaintEvent>
#include <QStandardItemModel>
#include <QStringList>

class QMenu;
class QListView;
class WidgetPopupMenu : public QWidget
{
    Q_OBJECT
public:
    enum Menu_Position { P_Top_Center = 1, P_Bottom_Center };
    explicit WidgetPopupMenu(QWidget *target);
    ~WidgetPopupMenu();

    void init();

    void setMenuList(const QStringList &list);

    void showMenu();

    void setColor(const QColor &color);

    void setSelectColor(const QColor &color);

    void setBackgroundColor(const QColor &color);

    void setMenuPosition(Menu_Position position);

signals:
    void sgl_menu_item_select(int index, const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;

    bool eventFilter(QObject *obj, QEvent *ev) override;

private slots:
    void slot_menu_item_click(const QModelIndex &index);

    QString parseColor(const QColor &color);

    void setStyle();

private:
    void closeMenu();

private:
    // 列表视图
    QListView *mListView = nullptr;

    // 菜单项
    QStandardItemModel *mMenuModel = nullptr;

    // 目标组件
    QWidget *mTargetWidgetPtr = nullptr;

    // 边距及圆角
    float mContentMargin = 8;

    // 文本最大长度
    int mMaxWidth = 0;

    // 文本色
    QColor mColor = {80, 86, 89};

    // 边框颜色
    QColor mBorderColor = {254, 254, 254};

    // 背景色
    QColor mBackgroundColor = {254, 254, 254};

    // 浮动颜色
    QColor mSelectColor = {230, 247, 255};

    // 方位
    int mMenuPosition = P_Top_Center;

    // 数量统计
    static int8_t mGlobalMenuCount;
};

#endif // WIDGETPOPUPMENU_H
