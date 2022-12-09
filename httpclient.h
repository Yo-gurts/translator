#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class HttpClient: public QObject
{
    Q_OBJECT
public:
    explicit HttpClient(QObject *parent = nullptr);

private:
    QString appId;                      /* 百度注册的 appId */
    QString key;                        /* 百度提供的 key */
    QString from;                       /* 指定翻译的源语言 */
    QString to;                         /* 指定翻译的目标语言 */
    QString prefix;                     /* 请求的地址 */
    QRandomGenerator qrand;             /* 用于生成随机数 */
    QCryptographicHash md5;             /* 用于计算 MD5 值 */
    QNetworkAccessManager *manager;     /* 用于发起请求 */
    QNetworkRequest request;            /* 用于发起请求 */

public slots:
    void doTranslate(QString text);     /* 处理 selected 事件 */

private slots:
    void replyFinished(QNetworkReply *);

signals:
    /* 通知已经获取到翻译结果 */
    void finshTranslate(QString str);
};

#endif // HTTPCLIENT_H
