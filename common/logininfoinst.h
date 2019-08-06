#ifndef LOGININFOINST_H
#define LOGININFOINST_H

#include <QString>

class LoginInfoInst {
public:
    void set(QString nickname = "", QString user = "",
             QString ip = "", QString port = "", QString token = "");
    static LoginInfoInst& getInstance();

    QString getUsername();
    QString getNickname();
    QString getServerIp();
    QString getServerPort();
    QString getToken();

private:
    LoginInfoInst() = default;
    LoginInfoInst(const LoginInfoInst&) = default;
    LoginInfoInst& operator=(const LoginInfoInst&) = default;
    ~LoginInfoInst() = default;

    static LoginInfoInst instance_;

private:
    QString nickname_;
    QString user_;
    QString ip_;
    QString port_;
    QString token_;
};

#endif // LOGININFOINST_H
