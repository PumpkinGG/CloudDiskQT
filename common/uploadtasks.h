#ifndef UPLOADTASKS_H
#define UPLOADTASKS_H

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

#include <QFile>
#include <QQueue>
#include <QHash>

#include "common/utils.h"
#include "common/logininfoinst.h"
#include "toolswt/dataprogress.h"

// 上传文件限制
const qint64 kMaxFileSize = 500*1024*1024;

// 上传文件信息
struct UploadFileInfo
{
    QString md5;        //文件md5码
    QFile *file;        //文件指针
    QString fileName;   //文件名字
    qint64 size;        //文件大小
    QString path;       //文件路径
    DataProgress *dp;   //上传进度控件
};

class UploadTasks
{
public:
    static UploadTasks* getInstance();

    // 添加任务
    AddTaskResult appendTask(QString path);

    // 取出任务
    UploadFileInfo* takeTask();

    // 删除任务
    void deleteTask(UploadFileInfo* uploadFile);

    // 清空任务列表
    void stop();

    // 正在退出
    bool isQuit();

    // 开启队列
    void start();

private:
    UploadTasks();
    ~UploadTasks() = default;

private:
    QMutex mutex_;
    QWaitCondition condition_;
    QQueue<UploadFileInfo*> files_;
    QHash<QString, UploadFileInfo*> isUploadFiles_;
    bool isQuit_;
};

#endif // UPLOADTASKS_H
