#include "logininfoinst.h"

LoginInfoInst LoginInfoInst::instance_;

void LoginInfoInst::set(QString nickname, QString user,
                        QString ip, QString port, QString token) {
    nickname_ = nickname;
    user_     = user;
    token_    = token;
    port_     = port;
    ip_       = ip;
}

LoginInfoInst& LoginInfoInst::getInstance()
{
    return instance_;
}

QString LoginInfoInst::getUsername() {
    return user_;
}

QString LoginInfoInst::getNickname() {
    return nickname_;
}

QString LoginInfoInst::getServerIp() {
    return ip_;
}

QString LoginInfoInst::getServerPort() {
    return port_;
}

QString LoginInfoInst::getToken() {
    return token_;
}
