#include "httpclient.h"

HttpClient::HttpClient(QObject *parent)
    : QObject{parent}, md5(QCryptographicHash::Md5)
{
    prefix = "http://api.fanyi.baidu.com/api/trans/vip/translate";
    appId = "20221116001455983";
    key = "g34rlaCKy3sabc4nZJ3H";
    from = "en";
    to = "zh";

    manager = new QNetworkAccessManager(this);
    request.setUrl(QUrl(prefix));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");

    connect(manager, &QNetworkAccessManager::finished,
                     this, &HttpClient::replyFinished);
}

/**
 * @brief 发起 POST 翻译请求
 * @param text 待翻译文本
 */
void HttpClient::doTranslate(QString text)
{
    QUrlQuery params;
    QString salt = QString::number(qrand.generate());
    QString sign = appId + text + salt + key;
    QByteArray signbyte = sign.toUtf8();

    md5.reset();
    md5.addData(signbyte);
    sign = md5.result().toHex();

    params.addQueryItem("q", text);
    params.addQueryItem("from", from);
    params.addQueryItem("to", to);
    params.addQueryItem("appid", appId);
    params.addQueryItem("salt", salt);
    params.addQueryItem("sign", sign);

    manager->post(request, params.query().toUtf8());
}

/**
 * @brief 翻译请求完成后的处理函数，发起 finshTranslate 信号，以便显示翻译结果
 * @param reply 请求的返回体
 */
void HttpClient::replyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString strReply = (QString)reply->readAll();
        qDebug() << strReply;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonObject jsonObj = jsonResponse.object();
        QJsonArray trans_result = jsonObj["trans_result"].toArray();
        const QJsonValue &trans = trans_result[0];

        emit finishTranslate(trans["dst"].toString());
    } else {
        qDebug() << "request error: " << reply->error();
    }
}

