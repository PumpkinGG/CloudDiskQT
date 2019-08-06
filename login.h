#ifndef LOGIN_H
#define LOGIN_H

#include "common/utils.h"
#include "mainwindow.h"
#include <QDialog>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_logupBtn_clicked();
    void on_loginBtn_clicked();
    void on_configBtn_clicked();
    void on_toLogupBtn_clicked();

    void on_titleWidget_showConfigPage();
    void on_titleWidget_minimizeWindow();
    void on_titleWidget_closeWindow();

private:
    // 重新登录
    void loginAgain();

    // 清空注册信息
    void clearLogupPage();

    // 初始化加载配置文件信息
    void initConfigInfo();

    // 封装Json格式注册信息
    QByteArray packLogupJson(QString user, QString nickname,
                             QString pwd, QString email);

    // 封装Json格式登录信息
    QByteArray packLoginJson(QString user, QString pwd);

private:
    Ui::Login *ui;
    QTimer *timer_;
    Utils *utils_;
    MainWindow* mainWindow_;

};

#endif // LOGIN_H
