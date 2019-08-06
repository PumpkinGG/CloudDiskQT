#ifndef UTILS_H
#define UTILS_H

#include <QDebug>
#include <QMap>
#include <QVariant>
#include <QListWidget>
#include <QFile>
#include <QNetworkAccessManager>

#define cout qDebug() << "[ " << __FILE__ << ":"  << __LINE__ << " ] "

const QString kConfigFile = "../config/config.json";  // 配置文件路径
const QString kRecordDir = "../config/record/";  // 用户文件上传下载记录
const QString kFiletypeDir = "../config/filetype/";   // 存放文件类型图片目录

/*--------------------------------\
| Url
|   %1 -- Server Ip
|   %2 -- Server Port
|   %3 -- Query String, command
`--------------------------------*/
const QString kLogupUrl = "http://%1:%2/logup";
const QString kLoginUrl = "http://%1:%2/login";
const QString kUploadMd5Url = "http://%1:%2/md5";
const QString kUploadUrl = "http://%1:%2/upload";
const QString kUserFileUrl = "http://%1:%2/userfile?cmd=%3";

// Server Reply Code
const QString kTokenInvalidCode = "110";
const QString kSuccessCode = "001";
const QString kFailedCode = "002";
const QString kUserExistedCode = "003";
const QString kMd5CheckFailCode = "004";
const QString kUserFileExistedCode = "005";

struct FileInfo {
    QString md5;               // 文件md5码
    QString filename;          // 文件名字
    QString user;              // 用户
    QString timestamp;         // 上传时间
    QString url;               // url
    QString type;              // 文件类型
    qint64 size;               // 文件大小
    int shareStatus;           // 是否共享, 1共享， 0不共享
    int downloadCount;         // 下载量
    QListWidgetItem* item;     // list widget 的item
};

enum AddTaskResult {
    kSuccess,
    kExistedInQueue,
    kOpenFileErr,
};

enum TransferStatus {
    kDownload,
    kUplaod,
    kRecord,
};

enum MainPages {
    kMyFile,
    kShareList,
    kRanking,
    kTransfer,
    kSwitchUser
};

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);
    ~Utils() override;

public:
    // 将配置信息保存到配置文件
    void writeConfigFile();

    // 从配置文件读取配置信息，若文件不存在则初始化配置信息
    void readConfigInfo();

    // 从配置信息中读取配置参数
    QString getConfigValue(QString field, QString key);

    // 将Login配置参数更新到配置信息中
    void setLoginInfo(QString user, QString pwd, bool isRemember);

    // 将Server配置参数更新到配置信息中
    void setServerInfo(QString ip, QString port);

    // AES Encryption, str <= 16 bytes
    static QString strEncryption(QString str);

    // AES Decryption
    static QString strDecryption(QString str);

    // String MD5
    static QByteArray getStrMd5(QString str);

    // String MD5
    static QByteArray getFileMd5(QString file);

    // 取得QNetworkAccessManager
    static QNetworkAccessManager* getNetworkAccessManager();

    // 取得服务器返回Code
    static QString getReplyCode(QByteArray data, QString key);

    // 初始化文件类型图标列表
    static void initTypeIconList();

    // 获取FileTypeIcon
    static QString getTypeIcon(QString type);

private:
    QMap<QString, QVariant> configInfo_;
    static QNetworkAccessManager manager_;
    static QStringList typeList_;
};

#endif // UTILS_H
