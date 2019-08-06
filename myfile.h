#ifndef MYFILE_H
#define MYFILE_H

#include <QWidget>
#include "common/utils.h"
#include "common/uploadtasks.h"
#include "toolswt/fileproperty.h"

namespace Ui {
class MyFile;
}

class MyFile : public QWidget
{
    Q_OBJECT

public:
    explicit MyFile(QWidget *parent = nullptr);
    ~MyFile();

    // 初始化文件列表ListWidget
    void initFileListWidget();

    // 添加右键菜单
    void addActionMenu();

    // 右键菜单事件
    void rightClickMenu(const QPoint &pos);

    // 添加上传文件项目item
    void addUploadItem(QString iconPath=":/resc/images/upload.png",
                  QString name="upload...");

    // 清空用户文件items, 负责QListWidgetItem的生命周期
    void clearFileItems();

    // 刷新文件控件列表
    void refreshFileItems();

    // 清空用户文件信息列表
    void clearFileList();

    // 刷新文件信息列表
    void refreshFileList();

    // 添加上传文件
    void addUploadFiles();

    // 开启上传任务线程
    void startUploadThread();

    // 停止上传下载任务线程
    void stopWorkThread();

    // 添加下载文件
    void addDownloadFile();

    // 开启下载任务线程
    void startDownloadThread();

    // 删除用户文件
    void deleteUserFile();

    // 显示文件信息
    void showUserFileProperty();

private:
    // 封装需发送Json数据
    QByteArray packGetUserFileJson(QString user, QString token);

    // 解析服务器返回Json数据
    void parseGetUserFileReplyJson(const QByteArray& data);

signals:
    void toTransfer(TransferStatus stat);
    void sigLoginAgain();

private:
    Ui::MyFile *ui;
    Utils utils_;
    // 文件列表
    QList<FileInfo*> files_;
    // 工作线程
    QList<QThread*> threads_;
    // 右键菜单
    QMenu   *pMenu_;           // 菜单
    QAction *pDownloadAct_;    // 下载
    QAction *pDeleteAct_;      // 删除
    QAction *pPropertyAct_;    // 属性
    // 文件属性Widget
    FileProperty* pFilePropertyWidget;
};

#endif // MYFILE_H
