#include "downloadthread.h"
#include "common/downloadtask.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>
#include <QEventLoop>

DownloadThread::DownloadThread(QObject *parent) :
    QObject(parent),
    manager_(new QNetworkAccessManager)
{
    manager_->setParent(this);
}

void DownloadThread::downloadWorkLoop()
{
    DownloadTask* task = DownloadTask::getInstance();
    while (!task->isQuit()) {
        DownloadInfo* downloadFileInfo = task->takeTask();
        if (downloadFileInfo != nullptr) {
            cout << "Download file" << downloadFileInfo->filename;
            this->downloadFile(downloadFileInfo);
        }
    }
}

void DownloadThread::downloadFile(DownloadInfo *downloadFileInfo)
{
    QNetworkRequest request;
    request.setUrl(downloadFileInfo->url);

    QNetworkReply* reply = manager_->get(request);
    if (reply == nullptr) {
        cout << "reply is null";
        return;
    }

    // 创建事件循环
    QEventLoop evl;
    connect(reply, &QNetworkReply::finished, &evl, &QEventLoop::quit);

    // 有可用数据更新时
    connect(reply, &QNetworkReply::uploadProgress,
            downloadFileInfo->dp, &DataProgress::setProgress);

    connect(reply, &QNetworkReply::finished, this, [=](){
        if (reply->error() != QNetworkReply::NoError) {
            cout << reply->errorString();
            reply->abort();
            reply->deleteLater();
            emit sigReplyError();
            emit sigDeleteTask(downloadFileInfo);
            return;
        }
        // Receive data
        QByteArray buff;
        buff.reserve(4096);
        do {
            buff = reply->read(4096);
            if (buff.size() == 0) {
                break;
            }
            downloadFileInfo->file->write(buff);
        } while(1);
        // 下载完成，记录&&删除任务
        cout << "Download file" << downloadFileInfo->filename << "success";
        reply->deleteLater(); // !!! 不要忘记释放资源，否则UI主线程会尝试释放报错
        emit sigDeleteTask(downloadFileInfo);
    });

    // 开启事件循环，等待Reply
    evl.exec();
}
