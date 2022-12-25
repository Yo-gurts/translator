#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    clip = QApplication::clipboard();
    onTop = false;

    /* 设置窗口样式 */
    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::Tool |
                   Qt::WindowStaysOnTopHint);
    setWindowOpacity(0.9);
    setStyleSheet("QMainWindow{background-color:rgba(255,255,255,255);"
                  "border-color:gray;border-radius:15px;}");

    /* 设置系统托盘 */
    systemTrayIcon.setIcon(QIcon(":/icon/tray.png"));
    systemTrayIcon.setToolTip("translator");
    subMenu = new QMenu("触发模式", &trayMenu);
    for (int i = 0; i < 4; i++) {
        mode[i] = new QAction(modeName[i], subMenu);
        mode[i]->setCheckable(true);    /* 设置为可点击 */
        subMenu->addAction(mode[i]);    /* 加入到子菜单中 */
        connect(mode[i], &QAction::triggered, this, &MainWindow::modeChange);
    }

    mode[0]->setChecked(true);    /* 设置为默认模式 */
    trayMenu.addMenu(subMenu);
    closeAction = new QAction("退出", &trayMenu);
    trayMenu.addAction(closeAction);
    systemTrayIcon.setContextMenu(&trayMenu);
    systemTrayIcon.show();

    connect(closeAction, &QAction::triggered, [](){
        qInfo() << "---------- Quit --------------";
        qApp->quit();
    });
    connect(ui->closeButton, &QPushButton::clicked, this, &MainWindow::doClose);
    connect(ui->copyButton, &QPushButton::clicked, this, &MainWindow::doCopy);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief 处理 finishTranslate 信号，将翻译结果显示在窗口中，若窗口出于可见状态，则不调整窗口位置。
 * @param str 翻译结果
 */
void MainWindow::showText(QString str)
{
    text = str;
    ui->copyed->hide();
    ui->textBrowser->setText(str);
    ui->copyButton->setStyleSheet("QPushButton{background-color:rgba(255,255,255,255);}");
    if (!onTop) {
        this->move(cursor.pos());
        this->show();
        onTop = true;
    }
    qDebug() << "showText: " << str;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    setCursor(Qt::CursorShape::PointingHandCursor);
    if (event->button() == Qt::LeftButton) {
        QPoint startPos = event->globalPos();
        cursorPos = startPos - geometry().topLeft();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        QPoint endPos = event->globalPos();
        move(endPos - cursorPos);
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    setCursor(Qt::CursorShape::ArrowCursor);
    QMainWindow::mouseReleaseEvent(event);
}

/**
 * @brief 将翻译结果复制到剪贴板
 */
void MainWindow::doCopy()
{
    ui->copyButton->setStyleSheet("QPushButton{background-color:rgba(180,180,180,1);}");
    clip->blockSignals(true);
    clip->setText(text);
    clip->blockSignals(false);
    qDebug() << "doCopy";
}

/**
 * @brief 关闭窗口（设置为不可见）
 */
void MainWindow::doClose()
{
    onTop = false;
    this->hide();
    qDebug() << "doClose";
}

/**
 * @brief 调整触发模式
 */
void MainWindow::modeChange()
{
    int i;
    QAction* newMode = qobject_cast<QAction*>(sender());
    if (!newMode)
        return;

    qDebug() << newMode->text();
    for (i = 0; i < 4; i++)
        mode[i]->setChecked(false);

    newMode->setChecked(true);
    for (i = 0; i < 4; i++) {
        if (newMode == mode[i]) {
            s->changeMode(i+1);
            break;
        }
    }
}
