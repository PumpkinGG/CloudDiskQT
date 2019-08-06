#include "uploadthread.h"
#include "uploadtasks.h"
#include "logininfoinst.h"
#include "utils.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QEventLoop>

UploadThread::UploadThread(QObject *parent) :
    QObject(parent),
    manager_(new QNetworkAccessManager)
{
    manager_->setParent(this);
}

void UploadThread::uploadWorkLoop()
{
    // 取得上传任务实例
    UploadTasks *uploadTasks = UploadTasks::getInstance();
    // Work Loop
    while (!uploadTasks->isQuit()) {
        UploadFileInfo *uploadFile = uploadTasks->takeTask();
        if (uploadFile != nullptr) {
            cout << "uploadFile " << uploadFile->fileName;
            tryUploadMd5(uploadFile);
        }
    }
}

void UploadThread::tryUploadMd5(UploadFileInfo *uploadFile)
{
    // 获取登陆信息实例
    LoginInfoInst& login = LoginInfoInst::getInstance();

    // Pack send data
    QByteArray sData = packUploadMd5Json(login.getUsername(), login.getToken(),
                                         uploadFile->md5, uploadFile->fileName);
    QNetworkRequest request;
    QUrl url = QString(kUploadMd5Url).arg(login.getServerIp())
                                     .arg(login.getServerPort());
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 发送post请求
    QNetworkReply *reply = manager_->post(request, sData);
    if(reply == nullptr) {
        cout << "reply is NULL";
        return;
    }

    // 创建事件循环
    QEventLoop evl;
    connect(reply, &QNetworkReply::finished, &evl, &QEventLoop::quit);

    connect(reply, &QNetworkReply::finished, this, [=](){
        if (reply->error() != QNetworkReply::NoError) {
            cout << reply->errorString();
            reply->abort();
            reply->deleteLater();
            emit sigDeleteTask(uploadFile);
            return;
        }
        // Solve reply data
        QByteArray replyData = reply->readAll();
        reply->deleteLater();
        QString code = Utils::getReplyCode(replyData, "code");
        if (kSuccessCode == code) {
            // MD5秒传成功
            cout << "MD5秒传成功";
        } else if (kUserFileExistedCode == code) {
            // 用户文件已存在，无需操作
            cout << "用户文件已存在，无需操作";
        } else if (kMd5CheckFailCode == code) {
            // 服务器没有此文件，需直接上传文件数据
            cout << "直接上传文件数据...";
            this->uploadExactly(uploadFile);
        } else if (kTokenInvalidCode == code) {
            cout << "Token is Invalid!";
            emit sigTokenInvalid(); //发送重新登陆信号
        } else {
            cout << "upload fail, code is " << code;
            emit sigReplyError();
        }

        if (kMd5CheckFailCode != code) {
            // 除需直接上传外，都要删除任务
            emit sigDeleteTask(uploadFile);
        }
    });

    // 开始循环，等待Reply
    evl.exec();
}

void UploadThread::uploadExactly(UploadFileInfo *uploadFile)
{
    LoginInfoInst& loginInfo = LoginInfoInst::getInstance();
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"file\" ");
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    filePart.setRawHeader("User", loginInfo.getUsername().toUtf8());
    filePart.setRawHeader("FileName", uploadFile->fileName.toUtf8());
    filePart.setRawHeader("FileSize", QString("%1").arg(uploadFile->size).toUtf8());
    filePart.setRawHeader("MD5", uploadFile->md5.toUtf8());

    if (!uploadFile->file->open(QFile::ReadOnly)) {
        cout << "open file err";
    }

    filePart.setBodyDevice(uploadFile->file);
    multiPart->append(filePart);

    QNetworkRequest request;
    QUrl url = QString(kUploadUrl).arg(loginInfo.getServerIp())
                                  .arg(loginInfo.getServerPort());
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");

    QNetworkReply *reply = manager_->post(request, multiPart);
    multiPart->setParent(reply);

    // 创建事件循环
    QEventLoop evl;
    connect(reply, &QNetworkReply::finished, &evl, &QEventLoop::quit);

    // 有可用数据更新时
    connect(reply, &QNetworkReply::uploadProgress,
            uploadFile->dp, &DataProgress::setProgress);

    connect(reply, &QNetworkReply::finished, this, [=](){
        if (reply->error() != QNetworkReply::NoError) {
            reply->abort();
            reply->deleteLater();
            cout << reply->errorString();
            emit sigDeleteTask(uploadFile);
            return;
        }
        // Solve reply data
        QByteArray replyData = reply->readAll();
        reply->deleteLater();
        QString code = Utils::getReplyCode(replyData, "code");
        if (kSuccessCode == code) {
            cout << "upload file success";
        } else {
            emit sigReplyError();
        }
        emit sigDeleteTask(uploadFile);
    });

    // 开启事件循环，等待Reply
    evl.exec();
}

QByteArray UploadThread::packUploadMd5Json(QString user, QString token,
                                           QString md5, QString fileName)
{
    QJsonObject root;
    root.insert("user", user);
    root.insert("token", token);
    root.insert("md5", md5);
    root.insert("filename", fileName);
    QJsonDocument doc(root);
    return doc.toJson();
}
