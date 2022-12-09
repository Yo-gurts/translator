#include "mainwindow.h"
#include "select.h"
#include "httpclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Select s;
    HttpClient cli;

    QObject::connect(&s, &Select::selected, &cli, &HttpClient::doTranslate);
    QObject::connect(&cli, &HttpClient::finshTranslate, &w, &MainWindow::showText);

    return a.exec();
}
