#include "filedownloader.h"

FileDownloader::FileDownloader(QUrl imageUrl, QObject *parent) :
    QObject(parent)
{
    qDebug() << "here" << __LINE__ << imageUrl;

    qDebug() << connect(
        &m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
        this, SLOT (fileDownloaded(QNetworkReply*))
    );

    QNetworkRequest request(imageUrl);
    qDebug() << request.url();
    m_WebCtrl.get(request);
}

FileDownloader::~FileDownloader() { }

void FileDownloader::fileDownloaded(QNetworkReply* pReply) {
    qDebug() << "here" << __LINE__;
    m_DownloadedData = pReply->readAll();
//emit a signal
    qDebug() << "here" << __LINE__;
    pReply->deleteLater();
    qDebug() << "here" << __LINE__;
    emit downloaded();
}

QByteArray FileDownloader::downloadedData() const {
    qDebug() << "here" << __LINE__;
    return m_DownloadedData;
}
