#include "uploadtasks.h"
#include "common/utils.h"
#include "common/uploadlayout.h"
#include "common/logininfoinst.h"
#include <QFileInfo>

UploadTasks *UploadTasks::getInstance()
{
    static UploadTasks instance_;
    return &instance_;
}

AddTaskResult UploadTasks::appendTask(QString path)
{
    QFileInfo info(path);
    qint64 size = info.size();

    QString fileName = info.fileName();
    if (isUploadFiles_.count(fileName) > 0) {
        cout << fileName << " 已经在上传队列中 ";
        return AddTaskResult::kExistedInQueue;
    }

    UploadFileInfo* uploadFile = new UploadFileInfo;
    uploadFile->file = new QFile(path);
    uploadFile->path = path;
    uploadFile->fileName = fileName;
    uploadFile->size = size;
    uploadFile->md5 = Utils::getFileMd5(path);

    DataProgress* dp = new DataProgress;
    dp->setFileName(fileName);
    dp->setProgress(0, size/1024);
    uploadFile->dp = dp;

    // 添加进度条到Upload布局
    UploadLayout& uploadLayout = UploadLayout::getInstance();
    QVBoxLayout* layout = uploadLayout.getUploadLayout();
    layout->insertWidget(layout->count()-1, dp);

    // 临界区
    {
        QMutexLocker lock(&mutex_);
        files_.push_back(uploadFile);
        isUploadFiles_.insert(fileName, uploadFile);
        condition_.notify_one();
    }

    return AddTaskResult::kSuccess;
}

UploadFileInfo *UploadTasks::takeTask()
{
    QMutexLocker lock(&mutex_);
    while (files_.size() == 0 && !isQuit_) {
        condition_.wait(&mutex_);
    }
    // 如果标记退出则返回nullptr
    if (isQuit()) {
        return nullptr;
    }
    UploadFileInfo* uploadFile = files_.front();
    files_.pop_front();
    return uploadFile;
}

void UploadTasks::deleteTask(UploadFileInfo* uploadFile)
{
    //获取布局，布局中移除控件
    UploadLayout& uploadLayout = UploadLayout::getInstance();
    QLayout *layout = uploadLayout.getUploadLayout();
    layout->removeWidget(uploadFile->dp);

    // 从正在上传Map里移除
    isUploadFiles_.remove(uploadFile->fileName);

    // 释放资源
    uploadFile->file->close();
    delete uploadFile->file;
    delete uploadFile->dp;
    delete uploadFile;
}

void UploadTasks::stop()
{
    QMutexLocker lock(&mutex_);
    for (const auto& file : files_) {
        deleteTask(file);
    }
    isQuit_ = true;
    condition_.notify_all();
}

bool UploadTasks::isQuit()
{
    return isQuit_;
}

void UploadTasks::start()
{
    isQuit_ = false;
}

UploadTasks::UploadTasks()
{
    isQuit_ = false;
}


