#ifndef SELECT_H
#define SELECT_H

#include <QApplication>
#include <QClipboard>
#include <QObject>
#include <QDebug>
#include <QPushButton>
#include <QCursor>
#include <QPoint>
#include <QTimer>
#include <QString>
#include <QDateTime>

class Select : public QObject
{
    Q_OBJECT
public:
    explicit Select(QObject *parent = nullptr);

private:
    QClipboard *clip;       /* 用于获取粘贴板内容、监听内容变化，单例模式 */
    qint64 lastStamp;       /* 上一次获取粘贴板内容的时间 ms */
    QPushButton *button;    /* 浮动按键，点击才翻译 */
    QCursor cursor;         /* 获取光标位置，显示按键 */
    QPoint pos;             /* 光标的当前位置 */
    QTimer timer;           /* 定时器，让按键超时消失 */
    QTimer selectTimer;     /* 定时器，防止鼠标选择过程中产生大量信号 */
    QString text;           /* 记录选中的字符串 */

private slots:
    void dataChanged();
    void clickButton();
    void timeout();
    void changed(QClipboard::Mode mode);
    void showButton();

signals:
    /* 通知需要进行翻译 */
    void selected(QString text);
};

#endif // SELECT_H
