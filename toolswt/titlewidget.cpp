#include "titlewidget.h"
#include "ui_titlewidget.h"
#include <QMouseEvent>

TitleWidget::TitleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleWidget),
    parent_(parent)
{
    ui->setupUi(this);
    // 设置logo、按钮图标
    ui->iconLabel->setPixmap(QPixmap(":/resc/images/logo.ico").scaled(38, 38));
    // Close button
    QObject::connect(ui->closeBtn, &QToolButton::clicked, this, [=](){
        emit this->closeWindow();
    });
    // Minimize button
    QObject::connect(ui->minimizeBtn, &QToolButton::clicked, this, [=](){
        emit this->minimizeWindow();
    });
    // Configure button
    QObject::connect(ui->toConfigBtn, &QToolButton::clicked, this, [=](){
        emit this->showConfigPage();
    });
}

TitleWidget::~TitleWidget()
{
    delete ui;
}

void TitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        parent_->move(event->globalPos() - posDiff_);
    }
}

void TitleWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 求按下位置相对界面左上角差值
        this->posDiff_ = event->globalPos() - parent_->geometry().topLeft();
    }
}
