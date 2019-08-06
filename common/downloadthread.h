#ifndef DOWNLOADTHREAD_H
#define DOWNLOADTHREAD_H

#include "downloadtask.h"
#include <QObject>
#include <QNetworkAccessManager>

class DownloadThread : public QObject
{
    Q_OBJECT
public:
    explicit DownloadThread(QObject *parent = nullptr);

    // 下载任务循环
    void downloadWorkLoop();

private:
    // 下载文件
    void downloadFile(DownloadInfo* downloadFile);

signals:
    // Send to MyFile
    void sigDeleteTask(DownloadInfo* downloadFile);
    void sigReplyError();

    // Send to DataProgress
    void sigUploadProgress(qint64 kbRead, qint64 totalKb);

public slots:

private:
    QNetworkAccessManager* manager_;
};

#endif // DOWNLOADTHREAD_H
