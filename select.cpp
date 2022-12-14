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

    /* 设置定时器只触发一次，而不是周期触发 */
    timer.setSingleShot(true);
    selectTimer.setSingleShot(true);

    /* 粘贴板中内容改变时，触发动作 */
//    QObject::connect(clip, &QClipboard::dataChanged, this, &Select::dataChanged);
    QObject::connect(button, &QPushButton::clicked, this, &Select::clickButton);
    QObject::connect(&timer, &QTimer::timeout, this, &Select::timeout);
    /* 选中的内容发改变时，触发动作 */
    QObject::connect(clip, &QClipboard::changed, this, &Select::changed);
    QObject::connect(&selectTimer, &QTimer::timeout, this, &Select::showButton);
}

/**
 * @brief 调整触发模式
 * @param mode
 */
void Select::changeMode(int mode)
{
    switch(mode) {
    case 1:
        disconnect(clip, &QClipboard::dataChanged, this, &Select::dataChanged);
        disconnect(&selectTimer, &QTimer::timeout, this, &Select::emitSelect);
        connect(clip, &QClipboard::changed, this, &Select::changed);
        connect(&selectTimer, &QTimer::timeout, this, &Select::showButton);
        break;
    case 2:
        disconnect(clip, &QClipboard::changed, this, &Select::changed);
        connect(clip, &QClipboard::dataChanged, this, &Select::dataChanged);
        break;
    case 3:
        disconnect(clip, &QClipboard::dataChanged, this, &Select::dataChanged);
        disconnect(&selectTimer, &QTimer::timeout, this, &Select::showButton);
        connect(clip, &QClipboard::changed, this, &Select::changed);
        connect(&selectTimer, &QTimer::timeout, this, &Select::emitSelect);
        break;
    case 4:
        disconnect(clip, &QClipboard::dataChanged, this, &Select::dataChanged);
        disconnect(clip, &QClipboard::changed, this, &Select::changed);
        break;
    }
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

/**
 * @brief 选择过程中会有clip::changed信号，处理该信号的函数
 */
void Select::changed(QClipboard::Mode mode)
{
    if (mode == QClipboard::Selection) {
        selectTimer.start(200);     /* 200 ms */
        pos = cursor.pos();
    }
}

/**
 * @brief 选中文字后，显示按钮
 */
void Select::showButton()
{
    QString str = clip->text(QClipboard::Selection);

    /* 针对 pdf 的内容 */
    str.replace("\r\n", " ");
    str.replace("\n", " ");

    if (str != "") {
        text = str;
        qDebug() << "showButton: " << str;

        button->move(pos);      /* 让按键显示在光标的位置 */
        timer.start(2000);      /* 开始 2 秒倒计时 */
        button->setVisible(true);
    }
}

/**
 * @brief 选中就翻译的模式下，处理数据的函数
 */
void Select::emitSelect()
{
    QString str = clip->text(QClipboard::Selection);

    /* 针对 pdf 的内容 */
    str.replace("\r\n", " ");
    str.replace("\n", " ");

    if (str != "") {
        text = str;
        qDebug() << "emitSelect: " << str;
        emit selected(text);
    }
}
