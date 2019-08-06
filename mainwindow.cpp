#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common/utils.h"
#include "common/logininfoinst.h"
#include "common/uploadtasks.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btnGroup->setParent(this);
    // 去除边框
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    // 设置图标
    this->setWindowIcon(QIcon(":/resc/images/logo.ico"));
    // 连接信号和槽
    makeConnects();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPixmap bk(":/resc/images/title_bk.jpg");
    painter.drawPixmap(0, 0, width(), height(), bk);
}

void MainWindow::makeConnects()
{
    // ButtonGroup connects
    connect(ui->btnGroup, &ButtonGroup::closeWindow, this, [=](){
        this->clearData();
        this->close();
    });
    connect(ui->btnGroup, &ButtonGroup::minWindow, this, [=](){
        this->showMinimized();
    });
    connect(ui->btnGroup, &ButtonGroup::maxWindow, this, [=](){
        this->showMaximized();
    });
    connect(ui->btnGroup, &ButtonGroup::normalWindow, this, [=](){
        this->showNormal();
    });
    connect(ui->btnGroup, &ButtonGroup::sigMyFile, this, [=](){
        ui->myfilePage->refreshFileList();
        ui->stackedWidget->setCurrentWidget(ui->myfilePage);
    });
    connect(ui->btnGroup, &ButtonGroup::sigShareList, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->sharelistPage);
    });
    connect(ui->btnGroup, &ButtonGroup::sigRanking, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->rankingPage);
    });
    connect(ui->btnGroup, &ButtonGroup::sigTransfer, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->transferPage);
    });
    connect(ui->btnGroup, &ButtonGroup::sigSwitchUser, this, [=](){
        this->clearData();
        emit sigSwitchUser();
    });

    connect(ui->myfilePage, &MyFile::toTransfer, this, [=](TransferStatus stat){
        ui->btnGroup->slotButtonClick(MainPages::kTransfer);
        ui->transferPage->setTable(stat);
    });
    connect(ui->myfilePage, &MyFile::sigLoginAgain, this, [=](){
        this->clearData();
        emit sigSwitchUser();
    });
}

void MainWindow::showMainWindow()
{
    LoginInfoInst& login = LoginInfoInst::getInstance();
    ui->btnGroup->setUserName(login.getNickname());
    ui->btnGroup->slotButtonClick(MainPages::kMyFile);
    ui->myfilePage->startUploadThread();
    ui->myfilePage->startDownloadThread();
    this->show();
}

void MainWindow::clearData()
{
    ui->myfilePage->clearFileItems();
    ui->myfilePage->clearFileList();
    ui->myfilePage->stopWorkThread();
}
