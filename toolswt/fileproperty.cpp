#include "fileproperty.h"
#include "ui_fileproperty.h"
#include <QIcon>

FileProperty::FileProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileProperty)
{
    ui->setupUi(this);
    this->setWindowTitle("Property");
    this->setWindowIcon(QIcon(":/resc/images/logo.ico"));
    this->setFixedSize(600, 400);
}

FileProperty::~FileProperty()
{
    delete ui;
}

void FileProperty::setFileInfo(FileInfo *fileInfo)
{
    QPixmap icon = QPixmap(Utils::getTypeIcon(fileInfo->type)).scaled(120, 120);
    ui->icon->setPixmap(icon);
    ui->fileName->setText(fileInfo->filename);
    ui->fileType->setText(fileInfo->type.toLower() + "文件");
    ui->timestamp->setText(fileInfo->timestamp);
    // File size
    QString fileSize;
    if (fileInfo->size > 1024 * 1024 * 1.2) {
        fileSize.append(QString::number(fileInfo->size/(1024.0*1024.0), 'f', 2))
                .append("MB");
    } else if (fileInfo->size > 1024 * 1.2) {
        fileSize.append(QString::number(fileInfo->size/(1024.0), 'f', 2))
                .append("KB");
    } else {
        fileSize.append(QString::number(fileInfo->size))
                .append("B");
    }
    ui->fileSize->setText(fileSize);
}
