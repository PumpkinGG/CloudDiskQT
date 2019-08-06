#ifndef UPLOADTHREAD_H
#define UPLOADTHREAD_H

#include "uploadtasks.h"
#include <QObject>
#include <QNetworkAccessManager>

class UploadThread : public QObject
{
    Q_OBJECT
public:
    explicit UploadThread(QObject *parent = nullptr);

    // 上传任务循环
    void uploadWorkLoop();

private:
    // 上传Md5，验证服务器端是否已有此文件
    void tryUploadMd5(UploadFileInfo *uploadFile);

    // 上传真实文件
    void uploadExactly(UploadFileInfo*);

    // 封装Md5 Json
    QByteArray packUploadMd5Json(QString user, QString token,
                                 QString md5, QString fileName);

signals:
    // Send to MyFile
    void sigTokenInvalid();
    void sigReplyError();
    void sigDeleteTask(UploadFileInfo *uploadFile);

    // Send to DataProgress
    void sigUploadProgress(qint64 kbRead, qint64 totalKb);

private slots:

private:
    QNetworkAccessManager* manager_;
};

#endif // UPLOADTHREAD_H
