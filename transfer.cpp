#include "transfer.h"
#include "ui_transfer.h"
#include "common/uploadlayout.h"
#include "common/downloadlayout.h"

Transfer::Transfer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Transfer)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentWidget(ui->record);

    // 设置样式 tabWidget
    ui->tabWidget->tabBar()->setStyleSheet(
       "QTabBar::tab{"
       "background-color: rgb(182, 202, 211);"
       "border-right: 1px solid gray;"
       "padding: 6px"
       "}"
       "QTabBar::tab:selected, QtabBar::tab:hover {"
       "background-color: rgb(20, 186, 248);"
       "}"
    );

    // 添加Upload布局
    UploadLayout& uploadLayout = UploadLayout::getInstance();
    uploadLayout.setUploadLayout(ui->uploadScroll);

    // 添加Download布局
    DownloadLayout& downloadLayout = DownloadLayout::getInstance();
    downloadLayout.setUploadLayout(ui->downloadScroll);
}

Transfer::~Transfer()
{
    delete ui;
}

void Transfer::setTable(TransferStatus stat)
{
    switch (stat) {
    case kUplaod :
        ui->tabWidget->setCurrentWidget(ui->upload);
        break;
    case kDownload :
        ui->tabWidget->setCurrentWidget(ui->download);
        break;
    case kRecord :
        ui->tabWidget->setCurrentWidget(ui->record);
        break;
    }
}
