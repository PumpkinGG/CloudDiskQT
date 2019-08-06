#include "downloadtask.h"
#include "common/utils.h"
#include "common/downloadlayout.h"

DownloadTask *DownloadTask::getInstance()
{
    static DownloadTask instance_;
    return &instance_;
}

AddTaskResult DownloadTask::appendTask(FileInfo *fileInfo, QString filePath)
{
    QString fileName = fileInfo->filename;
    if (isDownloadFiles_.count(fileName) > 0) {
        cout << fileName << "已经在下载队列中";
        return AddTaskResult::kExistedInQueue;
    }

    QFile* file = new QFile(filePath);
    file->open(QFile::WriteOnly);
    if (!file->isOpen()) {
        cout << fileName << "打开失败";
        delete file;
        return AddTaskResult::kOpenFileErr;
    }

    DownloadInfo* downloadFile = new DownloadInfo;
    downloadFile->file = file;
    downloadFile->filename = fileName;
    downloadFile->user = fileInfo->user;
    downloadFile->url = fileInfo->url;

    DataProgress* dp = new DataProgress;
    dp->setFileName(fileName);
    dp->setProgress(0, fileInfo->size/1024);
    downloadFile->dp = dp;

    // 添加进度条到Upload布局
    DownloadLayout& downloadLayout = DownloadLayout::getInstance();
    QVBoxLayout* layout = downloadLayout.getUploadLayout();
    layout->insertWidget(layout->count()-1, dp);

    // 临界区
    {
        QMutexLocker lock(&mutex_);
        files_.push_back(downloadFile);
        isDownloadFiles_.insert(fileInfo->filename, downloadFile);
        condition_.notify_one();
    }

    return AddTaskResult::kSuccess;
}

DownloadInfo *DownloadTask::takeTask()
{
    QMutexLocker lock(&mutex_);
    while (files_.size() == 0 && !isQuit_) {
        condition_.wait(&mutex_);
    }
    // 如果标记退出则返回nullptr
    if (isQuit()) {
        return nullptr;
    }
    DownloadInfo* downloadFile = files_.front();
    files_.pop_front();
    return downloadFile;
}

void DownloadTask::deleteTask(DownloadInfo *downloadFile)
{
    //获取布局，布局中移除控件
    DownloadLayout& downloadLayout = DownloadLayout::getInstance();
    QLayout *layout = downloadLayout.getUploadLayout();
    layout->removeWidget(downloadFile->dp);

    // 从正在下载Map里移除
    isDownloadFiles_.remove(downloadFile->filename);

    // 释放资源
    downloadFile->file->close();
    delete downloadFile->file;
    delete downloadFile->dp;
    delete downloadFile;
}

void DownloadTask::stop()
{
    QMutexLocker lock(&mutex_);
    for (const auto& file : files_) {
        deleteTask(file);
    }
    isQuit_ = true;
    condition_.notify_all();
}

bool DownloadTask::isQuit()
{
    return isQuit_;
}

void DownloadTask::start()
{
    isQuit_ = false;
}

DownloadTask::DownloadTask()
{
    isQuit_ = false;
}
