#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "select.h"

#include <QMainWindow>
#include <QPushButton>
#include <QClipboard>
#include <QCursor>
#include <QDebug>
#include <QLabel>
#include <QString>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QRadioButton>
#include <QButtonGroup>
#include <QAction>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Select *s;              /* 用于调整触发模式 */

public slots:
    void showText(QString str);

private:
    Ui::MainWindow *ui;
    QClipboard *clip;
    QCursor cursor;         /* 用于获取光标位置 */
    QString text;           /* 翻译结果 */
    bool onTop;             /* 标记当前窗口是否可见 */

    /* 系统托盘相关 */
    QSystemTrayIcon systemTrayIcon;
    QAction *closeAction;
    QMenu trayMenu;
    QMenu *subMenu;         /* 触发模式 子菜单 */
    QVector<QString> modeName = {"选中弹按钮", "复制弹按钮", "选中就翻译", "关闭所有"};
    QVector<QAction *> mode = {NULL, NULL, NULL, NULL};

    /* 鼠标点击点与窗口左上角的距离，用于拖动窗口 */
    QPoint cursorPos;

protected:
    /* 用于拖动窗口 */
    void mousePressEvent(QMouseEvent *event);   /* 鼠标点击事件 */
    void mouseMoveEvent(QMouseEvent *event);    /* 鼠标移动事件 */
    void mouseReleaseEvent(QMouseEvent *event); /* 鼠标释放事件 */

private slots:
    void doCopy();      /* 点击翻译按钮 */
    void doClose();     /* 点击复制按钮 */
    void modeChange();  /* 调整触发模式 */
};
#endif // MAINWINDOW_H
