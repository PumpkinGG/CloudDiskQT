#include "utils.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QMap>
#include <QVariant>
#include <QCryptographicHash>

#include <openssl/aes.h>

namespace {
const unsigned char kAesKey[AES_BLOCK_SIZE + 1] = "a921b3084C67dEF5";

} // namespace

// Singleton
QStringList Utils::typeList_ = QStringList();
QNetworkAccessManager Utils::manager_(nullptr);

Utils::Utils(QObject *parent) :
    QObject (parent)
{
    readConfigInfo();
}

Utils::~Utils()
{
    writeConfigFile();
}

void Utils::writeConfigFile()
{
    QFile file(kConfigFile);
    if (false == file.open(QFile::WriteOnly)) {
        cout << "Utils::writeConfigFile() open file err";
        return;
    }

    QJsonDocument doc = QJsonDocument::fromVariant(configInfo_);
    file.write(doc.toJson());
    file.close();
}

void Utils::readConfigInfo()
{
    QFile file(kConfigFile);
    if (false == file.open(QFile::ReadOnly)) {
        cout << "Utils::readConfigInfo() open file err";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || doc.isEmpty()) {
        return;
    }

    if (doc.isObject()) {
        QJsonObject fieldsObj = doc.object();
        QStringList fields = fieldsObj.keys();
        for(auto field : fields) {
            QJsonObject keysObj = fieldsObj.value(field).toObject();
            QStringList keys = keysObj.keys();
            QMap<QString, QVariant> temp;
            for (auto key : keys) {
                temp.insert(key, keysObj.value(key).toString());
            }
            configInfo_.insert(field, temp);
        }
    }
}

QString Utils::getConfigValue(QString field, QString key)
{
    if (configInfo_.isEmpty()) {
        return "";
    }

    if (0 == configInfo_.count(field)) {
        return "";
    }

    QMap<QString, QVariant> fieldMap = configInfo_[field].toMap();
    if (0 == fieldMap.count(key)) {
        return "";
    }

    return fieldMap[key].toString();
}

void Utils::setLoginInfo(QString user, QString pwd, bool isRemember)
{
    QMap<QString, QVariant> temp;
    temp.insert("user", user);
    temp.insert("pwd", pwd);
    temp.insert("remember", isRemember ? "Yes" : "No");
    configInfo_["login"] = temp;
}

void Utils::setServerInfo(QString ip, QString port)
{
    QMap<QString, QVariant> temp;
    temp.insert("ip", ip);
    temp.insert("port", port);
    configInfo_["server"] = temp;
}

QString Utils::strEncryption(QString str)
{
    const unsigned char* userKey = kAesKey;
    unsigned char in[AES_BLOCK_SIZE] = { 0 };
    unsigned char out[AES_BLOCK_SIZE+1] = { 0 };
    if (str.size() <= AES_BLOCK_SIZE) {
        memcpy(in, str.toLocal8Bit().data(), static_cast<size_t>(str.size()));
    } else {
        cout << "strEncryption() input size larger than 16";
        return "";
    }

    AES_KEY key;
    AES_set_encrypt_key(userKey, AES_BLOCK_SIZE * 8, &key);
    AES_encrypt(in, out, &key);
    QByteArray outBase64 = QByteArray(reinterpret_cast<char*>(out), AES_BLOCK_SIZE).toBase64();
    // cout << "Encryption: " << outBase64;
    return outBase64;
}

QString Utils::strDecryption(QString str)
{
    const unsigned char* userKey = kAesKey;
    unsigned char out[AES_BLOCK_SIZE+1] = { 0 };
    QByteArray in = QByteArray::fromBase64(str.toLocal8Bit().data());

    AES_KEY key;
    AES_set_decrypt_key(userKey, AES_BLOCK_SIZE * 8, &key);
    AES_decrypt(reinterpret_cast<unsigned char*>(in.data()), out, &key);
    QByteArray outStr = QByteArray(reinterpret_cast<char*>(out));
    return outStr;
}

QByteArray Utils::getStrMd5(QString str)
{
    QByteArray result = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5);
    return result.toHex();
}

QByteArray Utils::getFileMd5(QString path)
{
    QByteArray result;
    QFile file(path);
    file.open(QFile::ReadOnly);

    QCryptographicHash md5Hash(QCryptographicHash::Md5);
    if (md5Hash.addData(&file)) {
        result = md5Hash.result().toHex();
    }
    file.close();
    return result;
}

QNetworkAccessManager *Utils::getNetworkAccessManager()
{
    return &manager_;
}

QString Utils::getReplyCode(QByteArray data, QString key)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || doc.isEmpty()) {
        cout << "getReplyCode() data is Null or Empty";
        return "";
    }
    return doc.object().value(key).toString();
}

void Utils::initTypeIconList()
{
    QDir dir(kFiletypeDir);
    if (!dir.exists()) {
        cout << "Filetype Directory Doesn't Exist!";
        return;
    }

    dir.setFilter(QDir::Files | QDir::NoDot | QDir::NoDotDot | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList list = dir.entryInfoList();

    for (const auto& f : list) {
        typeList_.push_back(f.fileName());
    }
}

QString Utils::getTypeIcon(QString type)
{
    QString typeName = type.toLower() + ".png";
    if (typeList_.contains(typeName)) {
        return kFiletypeDir + typeName;
    } else {
        return kFiletypeDir + "other.png";
    }
}


