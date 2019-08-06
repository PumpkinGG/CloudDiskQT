#include "buttongroup.h"
#include "ui_buttongroup.h"
#include <QToolButton>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>

ButtonGroup::ButtonGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ButtonGroup),
    parent_(nullptr),
    curBtn_(nullptr)
{
    ui->setupUi(this);

    // 设置按钮信号映射
    btnTextMap_.insert(MainPages::kMyFile, ui->myfile->text());
    btnTextMap_.insert(MainPages::kShareList, ui->sharelist->text());
    btnTextMap_.insert(MainPages::kRanking, ui->ranking->text());
    btnTextMap_.insert(MainPages::kTransfer, ui->transfer->text());
    btnTextMap_.insert(MainPages::kSwitchUser, ui->switchuser->text());

    btns_.insert(ui->myfile->text(), ui->myfile);
    btns_.insert(ui->sharelist->text(), ui->sharelist);
    btns_.insert(ui->ranking->text(), ui->ranking);
    btns_.insert(ui->transfer->text(), ui->transfer);
    btns_.insert(ui->switchuser->text(), ui->switchuser);

    for (const auto& iter : btns_) {
        connect(iter, &QToolButton::clicked, this, [=](){
            slotButtonClick(iter->text());
        });
    }

    // 关闭
    connect(ui->close, &QToolButton::clicked, [=]() {
        emit closeWindow();
    });

    // 最大化
    connect(ui->max, &QToolButton::clicked, [=]() {
        static bool isMax = true;
        if(isMax) {
            emit maxWindow();
            ui->max->setIcon(QIcon(":/resc/images/title_normal.png"));
        }
        else {
            emit normalWindow();
            ui->max->setIcon(QIcon(":/resc/images/title_max.png"));
        }
        isMax = !isMax;
    });

    // 最小化
    connect(ui->min, &QToolButton::clicked, [=]() {
        emit minWindow();
    });
}

ButtonGroup::~ButtonGroup()
{
    delete ui;
}

void ButtonGroup::setParent(QWidget *parent)
{
    parent_ = parent;
}

void ButtonGroup::slotButtonClick(QString text)
{
    QToolButton* btn = btns_[text];
    if (btn == curBtn_ && btn != ui->switchuser) {
        return;
    }
    if (curBtn_ != nullptr) {
        curBtn_->setStyleSheet("color:black");
    }
    btn->setStyleSheet("color:red");
    curBtn_ = btn;
    // 发信号
    if (text == ui->myfile->text()) {
        emit sigMyFile();
    }
    else if (text == ui->sharelist->text()) {
        emit sigShareList();
    }
    else if (text == ui->ranking->text()) {
        emit sigRanking();
    }
    else if (text == ui->transfer->text()) {
        emit sigTransfer();
    }
    else if (text == ui->switchuser->text()) {
        emit sigSwitchUser();
    }
}

void ButtonGroup::slotButtonClick(MainPages page)
{
    QString text = btnTextMap_[page];
    slotButtonClick(text);
}

void ButtonGroup::setUserName(QString name)
{
    ui->login_usr->setText(name);
}

void ButtonGroup::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPixmap bk(":/resc/images/title_bk.jpg");
    painter.drawPixmap(0, 0, width(), height(), bk);
}

void ButtonGroup::mousePressEvent(QMouseEvent *event)
{
    // 如果是左键, 计算窗口左上角, 和当前按钮位置的距离
    if(event->button() == Qt::LeftButton) {
        // 计算和窗口左上角的相对位置
        posDiff_ = event->globalPos() - parent_->geometry().topLeft();
    }
}

void ButtonGroup::mouseMoveEvent(QMouseEvent *event)
{
    // 移动是持续的状态, 需要使用buttons
    if(event->buttons() & Qt::LeftButton) {
        parent_->move(event->globalPos() - posDiff_);
    }
}
