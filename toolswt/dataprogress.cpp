#include "dataprogress.h"
#include "ui_dataprogress.h"

DataProgress::DataProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataProgress)
{
    ui->setupUi(this);
}

DataProgress::~DataProgress()
{
    delete ui;
}

//设置文件名字
void DataProgress::setFileName(QString name)
{
    ui->name->setText(name + " : ");
    ui->info->setText("0 Byte / 0 Byte");
    ui->progressBar->setMinimum(0); //最小值
    ui->progressBar->setValue(0);
}

//设置进度条的当前值value, 最大值max
void DataProgress::setProgress(qint64 valueBytes, qint64 maxBytes)
{
    if (maxBytes == 0) return;
    ui->progressBar->setValue(static_cast<int>(valueBytes));
    ui->progressBar->setMaximum(static_cast<int>(maxBytes));
    QString infoText = QString("%1 MB / %2 MB").arg(valueBytes / (1024.0*1024.0), 0, 'f', 3)
                                               .arg(maxBytes / (1024.0*1024.0), 0, 'f', 3);
    ui->info->setText(infoText);
}
