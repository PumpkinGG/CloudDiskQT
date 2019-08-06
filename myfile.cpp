#include "myfile.h"
#include "ui_myfile.h"
#include "common/uploadtasks.h"
#include "common/uploadthread.h"
#include "common/downloadtask.h"
#include "common/downloadthread.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>
#include <QMenu>

MyFile::MyFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyFile),
    pFilePropertyWidget(new FileProperty(nullptr))
{
    ui->setupUi(this);
    // Init file type icons list
    Utils::initTypeIconList();
    // Init Right click menu
    this->addActionMenu();
    // Init user files list widget
    this->initFileListWidget();
}

MyFile::~MyFile()
{
    delete ui;
}

void MyFile::initFileListWidget()
{
    ui->fileList->setViewMode(QListView::IconMode);
    ui->fileList->setIconSize(QSize(100, 100));
    ui->fileList->setGridSize(QSize(150, 180));
    ui->fileList->setFont(QFont("Consolas"));
    // 设置QLisView大小改变时，图标的调整模式，默认是固定的，可以改成自动调整
    ui->fileList->setResizeMode(QListView::Adjust);   //自动适应布局
    // 设置图标不可拖动
    ui->fileList->setMovement(QListView::Static);
    // 设置右键菜单
    ui->fileList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->fileList, &QListWidget::customContextMenuRequested, this, &MyFile::rightClickMenu);
    // 连接上传控件点击事件
    connect(ui->fileList, &QListWidget::itemPressed, this, [=](QListWidgetItem* item) {
        QString str = item->text();
        if (str == "upload...") {
            //添加需要上传的文件到上传任务列表
            addUploadFiles();
        }
    });
}

void MyFile::addActionMenu()
{
    // RightButton Click Menu
    pMenu_ = new QMenu(this);
    // Menu Items
    pDownloadAct_ = new QAction("Download", this);
    pDeleteAct_ = new QAction("Delete", this);
    pPropertyAct_ = new QAction("Property", this);
    // Add items to menu
    pMenu_->addAction(pDownloadAct_);
    pMenu_->addAction(pDeleteAct_);
    pMenu_->addAction(pPropertyAct_);

    // Connects
    connect(pDownloadAct_, &QAction::triggered, this, &MyFile::addDownloadFile);
    connect(pDeleteAct_, &QAction::triggered, this, &MyFile::deleteUserFile);
    connect(pPropertyAct_, &QAction::triggered, this, &MyFile::showUserFileProperty);
}

void MyFile::rightClickMenu(const QPoint &pos)
{
    // Get clicked item
    QListWidgetItem *item = ui->fileList->itemAt(pos);
    if (item != nullptr) {
        ui->fileList->setCurrentItem(item);
        if (item->text() == "upload...") {
            return;
        }
        pMenu_->exec(QCursor::pos());
    }
}

void MyFile::addUploadItem(QString iconPath, QString name)
{
    ui->fileList->addItem(new QListWidgetItem(QIcon(iconPath), name));
}

void MyFile::clearFileItems()
{
    int count = ui->fileList->count();
    for (int i = 0; i < count; i++) {
        auto item = ui->fileList->takeItem(0);
        delete item;
    }
}

void MyFile::refreshFileItems()
{
    this->clearFileItems();
    // Add FileInfos
    for (const auto& file : files_) {
        ui->fileList->addItem(file->item);
    }
    this->addUploadItem();
}

void MyFile::clearFileList()
{
    while (!files_.isEmpty()) {
        FileInfo* file = files_.takeFirst();
        delete file;
    }
}

void MyFile::refreshFileList()
{
    // 取得Login实例
    LoginInfoInst& login = LoginInfoInst::getInstance();
    QNetworkAccessManager* manager = Utils::getNetworkAccessManager();

    QByteArray sData = this->packGetUserFileJson(login.getUsername(),
                                                 login.getToken());
    QNetworkRequest request;
    QString url = QString(kUserFileUrl).arg(login.getServerIp())
                                       .arg(login.getServerPort())
                                       .arg("list");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = manager->post(request, sData);
    connect(reply, &QNetworkReply::finished, this, [=](){
        if (reply->error() != QNetworkReply::NoError) {
            reply->abort();
            reply->deleteLater();
            cout << reply->errorString();
            return;
        }
        // Deal with reply data
        QByteArray replyData = reply->readAll();
        reply->deleteLater();

        QString code = Utils::getReplyCode(replyData, "code");
        if (kTokenInvalidCode == code) {
            emit sigLoginAgain();
        } else if (kFailedCode == code) {
            QMessageBox::warning(this, "Error", "获取用户文件列表失败！");
        } else if (kSuccessCode == code) {
            this->clearFileList();
            parseGetUserFileReplyJson(replyData);
            this->refreshFileItems();
        }
    });
}

void MyFile::addUploadFiles()
{
    emit toTransfer(TransferStatus::kUplaod);
    // 取得上传任务实例
    UploadTasks *uploadTasks = UploadTasks::getInstance();
    // 取得上传文件列表
    QStringList files = QFileDialog::getOpenFileNames(this, "上传文件");
    // 添加任务
    for (const auto& file : files) {
        AddTaskResult result = uploadTasks->appendTask(file);
        // 处理结果
        switch (result) {
        case AddTaskResult::kSuccess :
            cout << file << "added to upload list";
            break;
        case AddTaskResult::kExistedInQueue :
            QMessageBox::warning(this, "Error", "上传的文件已经在上传队列中！");
            break;
        case AddTaskResult::kOpenFileErr :
            cout << "Open file error";
            break;
        }
    }
}

void MyFile::startUploadThread()
{
    UploadTasks* tasks = UploadTasks::getInstance();
    tasks->start();

    QThread* thread = new QThread;
    threads_.push_back(thread);
    UploadThread* uploadThread = new UploadThread;
    uploadThread->moveToThread(thread);
    // Connect main loop
    connect(thread, &QThread::started,
            uploadThread, &UploadThread::uploadWorkLoop);
    // Connect deleteLater
    connect(thread, &QThread::finished, this, [=](){
        thread->deleteLater();
        delete uploadThread;
    });
    // Connect signals
    connect(uploadThread, &UploadThread::sigTokenInvalid,
            this, &MyFile::sigLoginAgain);
    connect(uploadThread, &UploadThread::sigReplyError,
            this, [=](){
        QMessageBox::warning(this, "Error", "上传文件失败！");
    });
    connect(uploadThread, &UploadThread::sigDeleteTask,
            this, [=](UploadFileInfo* fileInfo){
        tasks->deleteTask(fileInfo);
    });

    // Start thread
    thread->start();
}

void MyFile::stopWorkThread()
{
    UploadTasks* uploadTasks = UploadTasks::getInstance();
    uploadTasks->stop();
    DownloadTask* downloadTask = DownloadTask::getInstance();
    downloadTask->stop();

    for (const auto& thread : threads_) {
        thread->exit();
        thread->wait();
    }
    threads_.clear();
}

void MyFile::addDownloadFile()
{
    emit toTransfer(TransferStatus::kDownload);
    // 取得下载任务实例
    DownloadTask *downloadTask = DownloadTask::getInstance();
    // 取得当前点击文件
    QString fileName = ui->fileList->currentItem()->text();
    FileInfo* file = nullptr;
    for (const auto& f : files_) {
        if (f->filename == fileName) {
            file = f;
            break;
        }
    }
    // 取得保存文件路径
    QString path = QFileDialog::getSaveFileName(this, "保存文件", fileName);
    AddTaskResult result = downloadTask->appendTask(file, path);
    switch (result) {
    case AddTaskResult::kSuccess :
        cout << fileName << "added to download list";
        break;
    case AddTaskResult::kExistedInQueue :
        QMessageBox::warning(this, "Error", "下载的文件已经在下载队列中！");
        break;
    case AddTaskResult::kOpenFileErr :
        cout << "Open file error";
        break;
    }
}

void MyFile::startDownloadThread()
{
    DownloadTask* tasks = DownloadTask::getInstance();
    tasks->start();

    QThread* thread = new QThread;
    threads_.push_back(thread);
    DownloadThread* downloadThread = new DownloadThread;
    downloadThread->moveToThread(thread);
    // Connect main loop
    connect(thread, &QThread::started,
            downloadThread, &DownloadThread::downloadWorkLoop);
    // Connect deleteLater
    connect(thread, &QThread::finished, this, [=](){
        thread->deleteLater();
        delete downloadThread;
    });
    // Connect signals
    connect(downloadThread, &DownloadThread::sigReplyError,
            this, [=](){
        QMessageBox::warning(this, "Error", "下载文件失败！");
    });
    connect(downloadThread, &DownloadThread::sigDeleteTask,
            this, [=](DownloadInfo* fileInfo){
        tasks->deleteTask(fileInfo);
    });

    // Start thread
    thread->start();
}

void MyFile::deleteUserFile()
{
    cout << "Delete user file" << ui->fileList->currentItem()->text();
}

void MyFile::showUserFileProperty()
{
    // 取得当前点击文件
    QString fileName = ui->fileList->currentItem()->text();
    FileInfo* file = nullptr;
    for (const auto& f : files_) {
        if (f->filename == fileName) {
            file = f;
            break;
        }
    }
    pFilePropertyWidget->setFileInfo(file);
    pFilePropertyWidget->exec();
}

QByteArray MyFile::packGetUserFileJson(QString user, QString token)
{
    QJsonObject root;
    root.insert("user", user);
    root.insert("token", token);

    QJsonDocument doc = QJsonDocument(root);
    return doc.toJson();
}

void MyFile::parseGetUserFileReplyJson(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || doc.isEmpty()) {
        cout << "replyData is Null or Empty";
        return;
    }

    /*
     *  fileInfo["user"] = file.user;
     *  fileInfo["filename"] = file.filename;
     *  fileInfo["timestamp"] = file.timestamp;
     *  fileInfo["shareStatus"] = file.shareStatus;
     *  fileInfo["downloadCount"] = file.downloadCount;
     *  fileInfo["url"] = file.url;
     *  fileInfo["type"] = file.type;
     *  fileInfo["size"] = file.size;
     */
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("files")) {
            QJsonArray filesArray = obj.value("files").toArray();
            for (auto fileVal : filesArray) {
                QJsonObject fileObj = fileVal.toObject();
                FileInfo* pFile = new FileInfo;
                pFile->user = fileObj.value("user").toString();
                pFile->filename = fileObj.value("filename").toString();
                pFile->timestamp = fileObj.value("timestamp").toString();
                pFile->shareStatus = fileObj.value("shareStatus").toInt();
                pFile->downloadCount = fileObj.value("downloadCount").toInt();
                pFile->url = fileObj.value("url").toString();
                pFile->type = fileObj.value("type").toString();
                pFile->size = fileObj.value("size").toInt();
                pFile->item = new QListWidgetItem(QIcon(Utils::getTypeIcon(pFile->type)),
                                                  pFile->filename);
                files_.append(pFile);
            }
        }
    }
}
