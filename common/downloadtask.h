#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

#include <QFile>
#include <QQueue>
#include <QHash>

#include "common/utils.h"
#include "common/logininfoinst.h"
#include "toolswt/dataprogress.h"

//下载文件信息
struct DownloadInfo
{
    QFile *file;        //文件指针
    QString user;       //下载用户
    QString filename;   //文件名字
    QUrl url;           //下载网址
    DataProgress *dp;   //下载进度控件
};

class DownloadTask
{
public:
    static DownloadTask* getInstance();

    // 添加任务
    AddTaskResult appendTask(FileInfo* fileInfo, QString filePath);

    // 取出任务
    DownloadInfo* takeTask();

    // 删除任务
    void deleteTask(DownloadInfo* downloadFile);

    // 清空任务列表
    void stop();

    // 正在退出
    bool isQuit();

    // 开启队列
    void start();

private:
    DownloadTask();
    ~DownloadTask() = default;

private:
    QMutex mutex_;
    QWaitCondition condition_;
    QQueue<DownloadInfo*> files_;
    QHash<QString, DownloadInfo*> isDownloadFiles_;
    bool isQuit_;
};

#endif // DOWNLOADTASK_H
