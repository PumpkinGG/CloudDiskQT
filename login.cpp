#include "login.h"
#include "ui_login.h"
#include "common/utils.h"
#include "common/logininfoinst.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QRegExp>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>

#include <openssl/aes.h>

namespace {
// 正则表达式
const QString kUserReg = "^[a-zA-Z\\d_@#-\\*]{3,16}$";
const QString kPasswdReg = "^[a-zA-Z\\d_@#-\\*]{6,16}$";
const QString kPhoneReg = "^1\\d{10}$";
const QString kEmailReg = "^[a-zA-Z0-9._-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)*\\.[a-zA-Z0-9]{2,6}$";
const QString kIpReg = "^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$";
const QString kPortReg = "^([1-9]\\d{0,3})|([1-5]\\d{4})|(6[0-4]\\d{3})|(65[0-4]\\d{2})|(655[0-2]\\d{1})|(6553[0-5])$";
const int kTimerInterval = 8000;

} // namespace


Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login),
    timer_(new QTimer),
    utils_(new Utils)
{
    ui->setupUi(this);
    utils_->setParent(this);
    timer_->setParent(this);

    // 创建并设置MainWindow
    mainWindow_ = new MainWindow();
    connect(mainWindow_, &MainWindow::sigSwitchUser,
            this, &Login::loginAgain);
    // 去除边框
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    // 设置图标
    this->setWindowIcon(QIcon(":/resc/images/logo.ico"));
    // 初始化配置信息
    this->initConfigInfo();
    // 设置默认登录页
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    ui->userLogin->setFocus();
    ui->userLogin->selectAll();
    // 设置更改用户名则清空密码
    connect(ui->userLogin, &QLineEdit::textEdited, this, [=](){
        ui->pwdLogin->clear();
    });
}

Login::~Login()
{
    delete ui;
}

void Login::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    // Draw background
    QPainter p(this);
    QPixmap background(":/resc/images/login_bk.jpg");
    p.drawPixmap(0, 0, width(), height(), background);
}

void Login::clearLogupPage()
{
    ui->userLogup->clear();
    ui->nicknameLogup->clear();
    ui->pwdLogup->clear();
    ui->confirmPwdLogup->clear();
    ui->emailLogup->clear();
}

void Login::initConfigInfo()
{
    // Read server config info
    ui->serverIp->setText(utils_->getConfigValue("server", "ip"));
    ui->serverPort->setText(utils_->getConfigValue("server", "port"));
    // Read login info
    // @TODO: encryption algorithm
    QString user = utils_->getConfigValue("login", "user");
    QString pwd = utils_->getConfigValue("login", "pwd");
    bool isRemember = (utils_->getConfigValue("login", "remember") == "Yes");
    if (!user.isEmpty()) {
        user = Utils::strDecryption(user);
    }
    if (!pwd.isEmpty()) {
        pwd = Utils::strDecryption(pwd);
    }
    ui->userLogin->setText(user);
    ui->pwdLogin->setText(pwd);
    ui->pwdCheckBoxLogin->setChecked(isRemember);
}

QByteArray Login::packLogupJson(QString user, QString nickname, QString pwd, QString email)
{
    QJsonObject logup;
    logup.insert("user", user);
    logup.insert("nickname", nickname);
    logup.insert("password", pwd);
    logup.insert("email", email);

    QJsonDocument doc = QJsonDocument(logup);
    return doc.toJson();
}

QByteArray Login::packLoginJson(QString user, QString pwd)
{
    QJsonObject login;
    login.insert("user", user);
    login.insert("password", pwd);

    QJsonDocument doc = QJsonDocument(login);
    return doc.toJson();
}

void Login::on_logupBtn_clicked()
{
    ui->logupBtn->setEnabled(false);

    QString user = ui->userLogup->text();
    QString nickname = ui->nicknameLogup->text();
    QString pwd = ui->pwdLogup->text();
    QString confirm = ui->confirmPwdLogup->text();
    QString email = ui->emailLogup->text();
    QRegExp reg;

    // Check server info
    QString ip = ui->serverIp->text();
    QString port = ui->serverPort->text();
    if (ip.isEmpty() || port.isEmpty()) {
        QMessageBox::information(this, "Info", "未设置服务器参数");
        ui->stackedWidget->setCurrentWidget(ui->configPage);
        ui->serverIp->setFocus();
        ui->serverIp->selectAll();
        ui->logupBtn->setEnabled(true);
        ui->pwdLogup->clear();
        ui->confirmPwdLogup->clear();
    }

    // Check info
    reg.setPattern(kUserReg);
    if (!reg.exactMatch(user)) {
        QMessageBox::warning(this, "Error", "用户名可包含a-z,A-Z,数字,_@#-*,长度{3,16}");
        ui->userLogup->setFocus();
        ui->userLogup->selectAll();
        return;
    }
    reg.setPattern(kPasswdReg);
    if (!reg.exactMatch(pwd)) {
        QMessageBox::warning(this, "Error", "密码可包含a-z,A-Z,数字,_@#-*,长度{6,16}");
        ui->pwdLogup->setFocus();
        ui->pwdLogup->selectAll();
        return;
    }
    if (pwd != confirm) {
        QMessageBox::warning(this, "Error", "确认密码不一致");
        ui->confirmPwdLogup->setFocus();
        ui->confirmPwdLogup->selectAll();
        return;
    }
    reg.setPattern(kEmailReg);
    if (!reg.exactMatch(email)) {
        QMessageBox::warning(this, "Error", "邮箱格式不正确");
        ui->emailLogup->setFocus();
        ui->emailLogup->selectAll();
        return;
    }

    //
    // logup && user message reply
    //
    QByteArray sData = this->packLogupJson(user, nickname, pwd, email);
    QNetworkAccessManager* manager = Utils::getNetworkAccessManager();

    QString urlStr = QString(kLogupUrl).arg(ip).arg(port);
    QNetworkRequest request;
    request.setUrl(QUrl(urlStr));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, sData.size());

    QNetworkReply* reply = manager->post(request, sData);
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // Stop timer
        disconnect(timer_, &QTimer::timeout, nullptr, nullptr);
        timer_->stop();
        // Check error
        if (reply->error() != QNetworkReply::NoError) {
            reply->abort();
            reply->deleteLater();
            cout << reply->errorString();
            ui->logupBtn->setEnabled(true);
            return;
        }
        // Deal with reply message
        QByteArray replyData = reply->readAll();
        reply->deleteLater();

        QString code = Utils::getReplyCode(replyData, "code");
        if (kSuccessCode == code) {
            QMessageBox::information(this, "Success", "注册成功");
            ui->stackedWidget->setCurrentWidget(ui->loginPage);
            ui->userLogin->setText(user);
            ui->pwdLogin->clear();
            ui->pwdLogin->setFocus();
            this->clearLogupPage();
        } else if (kUserExistedCode == code) {
            QMessageBox::warning(this, "Error", "用户已存在");
        } else if (kFailedCode == code) {
            QMessageBox::warning(this, "Error", "注册失败");
        } else {
            QMessageBox::warning(this, "Error", "服务器回复未知错误");
        }

        ui->logupBtn->setEnabled(true);
    });

    // Set timer
    timer_->start(kTimerInterval);
    connect(timer_, &QTimer::timeout, this, [=](){
        timer_->stop();
        disconnect(reply, &QNetworkReply::readyRead, nullptr, nullptr);
        QMessageBox::warning(this, "Error", "服务器未响应");
        reply->abort();
        reply->deleteLater();
        ui->logupBtn->setEnabled(true);
    });
}

void Login::on_loginBtn_clicked()
{
    ui->loginBtn->setEnabled(false);

    QString user = ui->userLogin->text();
    QString pwd  = ui->pwdLogin->text();
    bool isRemember = ui->pwdCheckBoxLogin->isChecked();
    QString ip = ui->serverIp->text();
    QString port = ui->serverPort->text();

    // Check server info
    if (ip.isEmpty() || port.isEmpty()) {
        QMessageBox::information(this, "Info", "未设置服务器参数");
        ui->stackedWidget->setCurrentWidget(ui->configPage);
        ui->serverIp->setFocus();
        ui->serverIp->selectAll();
        ui->loginBtn->setEnabled(true);
    }

    //
    // @TODO: login code
    // return: success - do next
    //         fail - send message to user, then return
    //
    QByteArray message = this->packLoginJson(user, Utils::getStrMd5(pwd));
    QNetworkAccessManager* network = Utils::getNetworkAccessManager();

    QString urlStr = QString(kLoginUrl).arg(ip).arg(port);
    QNetworkRequest request;
    request.setUrl(QUrl(urlStr));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, message.size());

    QNetworkReply* reply = network->post(request, message);
    connect(reply, &QNetworkReply::readyRead, this, [=](){
        // Stop timer
        disconnect(timer_, &QTimer::timeout, nullptr, nullptr);
        timer_->stop();
        // Check error
        if (reply->error() != QNetworkReply::NoError) {
            reply->abort();
            reply->deleteLater();
            cout << reply->errorString();
            ui->loginBtn->setEnabled(true);
            return;
        }
        // Deal with reply message
        QByteArray replyData = reply->readAll();
        reply->deleteLater();

        QString code = Utils::getReplyCode(replyData, "code");
        if (kSuccessCode == code) {
            QString token = Utils::getReplyCode(replyData, "token");
            QString nickname = Utils::getReplyCode(replyData, "nickname");
            // Init LoginInfoInst
            LoginInfoInst& loginInst = LoginInfoInst::getInstance();
            loginInst.set(nickname, user, ip, port, token);
            // Debug
            cout << endl
                 << "Nickname: " << loginInst.getNickname() << endl
                 << "ServerIp: " << loginInst.getServerIp();
            // Switch windows
            this->hide();
            mainWindow_->showMainWindow();

        } else if (kFailedCode == code) {
            QMessageBox::warning(this, "Error", "登录失败");
        } else {
            QMessageBox::warning(this, "Error", "服务器回复未知错误");
        }

        ui->loginBtn->setEnabled(true);
    });

    // Set timer
    timer_->start(kTimerInterval);
    connect(timer_, &QTimer::timeout, this, [=](){
        timer_->stop();
        disconnect(reply, &QNetworkReply::readyRead, nullptr, nullptr);
        QMessageBox::warning(this, "Error", "服务器未响应");
        reply->abort();
        reply->deleteLater();
        ui->loginBtn->setEnabled(true);
    });

    // Write login info
    if (isRemember) {
        QString encodeUser = Utils::strEncryption(user);
        QString encodePwd = Utils::strEncryption(pwd);
        utils_->setLoginInfo(encodeUser, encodePwd, isRemember);
    } else {
        QString encodeUser = Utils::strEncryption(user);
        utils_->setLoginInfo(encodeUser, "", isRemember);
    }
}

void Login::on_configBtn_clicked()
{
    QString ip = ui->serverIp->text();
    QString port = ui->serverPort->text();
    // Check ip pattern
    QRegExp reg(kIpReg);
    if (!reg.exactMatch(ip)) {
        QMessageBox::warning(this, "Error", "Ip格式不正确");
        ui->serverIp->setFocus();
        ui->serverIp->selectAll();
        return;
    }
    // Check port pattern
    reg.setPattern(kPortReg);
    if (!reg.exactMatch(port)) {
        QMessageBox::warning(this, "Error", "端口号格式不正确");
        ui->serverPort->setFocus();
        ui->serverPort->selectAll();
        return;
    }

    // 保存配置信息
    utils_->setServerInfo(ip, port);

    QMessageBox::information(this, "Success", "设置成功。");
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    ui->userLogin->setFocus();
    ui->userLogin->selectAll();
}

void Login::on_toLogupBtn_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->logupPage);
    ui->userLogup->setFocus();
    ui->logupBtn->setEnabled(true);
}

void Login::on_titleWidget_showConfigPage()
{
    ui->stackedWidget->setCurrentWidget(ui->configPage);
    ui->serverIp->selectAll();
    ui->serverIp->setFocus();
    ui->configBtn->setEnabled(true);
}

void Login::on_titleWidget_minimizeWindow()
{
    this->showMinimized();
}

void Login::on_titleWidget_closeWindow()
{
    if (ui->stackedWidget->currentWidget() != ui->loginPage) {
        this->clearLogupPage();
        ui->stackedWidget->setCurrentWidget(ui->loginPage);
        ui->userLogin->setFocus();
        ui->userLogin->selectAll();
        ui->loginBtn->setEnabled(true);
    } else {
        this->close();
    }
}

void Login::loginAgain()
{
    this->mainWindow_->hide();
    // 设置默认登录页
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    ui->userLogin->setFocus();
    ui->userLogin->selectAll();
    ui->pwdLogin->clear();
    this->show();
}
