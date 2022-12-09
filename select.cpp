#include "select.h"

Select::Select(QObject *parent)
    : QObject{parent}
{
    clip = QApplication::clipboard();
    lastStamp = 0;

    /* 设置button样式，圆角 */
    button = new QPushButton();
    button->setWindowFlags(Qt::FramelessWindowHint |    /* 无边框 */
                           Qt::WindowStaysOnTopHint |   /* 显示在最顶层 */
                           Qt::Tool | Qt::X11BypassWindowManagerHint);

    button->setFixedSize(50, 50);
    button->setAttribute(Qt::WA_TranslucentBackground); /* 半透明 */
    button->setStyleSheet("QPushButton{width: 200px; height: 50px;"
                          "background-color:rgba(255,255,255,1);"
                          "border-image: url(:/icon/button.png) 10 10 10 10 stretch stretch;"
                          "border-radius: 10px;}"
                          "QPushButton:hover{background-color:rgba(240,240,240,1);}"
                          "QPushButton:pressed{background-color:rgba(200,200,200,1);}");

    /* 粘贴板中内容改变时，触发动作 */
    QObject::connect(clip, &QClipboard::dataChanged, this, &Select::dataChanged);
    QObject::connect(button, &QPushButton::clicked, this, &Select::clickButton);
    QObject::connect(&timer, &QTimer::timeout, this, &Select::timeout);
}

/**
 * @brief 剪贴板内容发送变化，需要显示按键，并对字符串进行预处理。
 */
void Select::dataChanged()
{
    QString str = clip->text();
    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch(); /* ms */

    /* 针对 pdf 的复制内容 */
    str.replace("\r\n", " ");
    str.replace("\n", " ");

    /* 针对 WPS for Linux 复制时异常发出4此信号 */
    if (str != "" && now - lastStamp > 300) {
        text = str;
        lastStamp = now;
        qDebug() << "dataChanged: " << str;

        pos = cursor.pos();
        button->move(pos);      /* 让按键显示在光标的位置 */
        timer.start(2000);      /* 开始 2 秒倒计时 */
        button->setVisible(true);
    }
}

/**
 * @brief 按钮点击后，设置按钮不可见，发起 selected 信号
 */
void Select::clickButton()
{
    timer.stop();
    button->setVisible(false);
    emit selected(text);
}

/**
 * @brief 定时器超时处理，隐藏按钮
 */
void Select::timeout()
{
    button->setVisible(false);
}
