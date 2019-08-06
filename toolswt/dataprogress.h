#ifndef DATAPROGRESS_H
#define DATAPROGRESS_H

#include <QWidget>

namespace Ui {
class DataProgress;
}

//上传，下载进度控件
class DataProgress : public QWidget
{
    Q_OBJECT

public:
    explicit DataProgress(QWidget *parent = nullptr);
    ~DataProgress();

    //设置文件名字
    void setFileName(QString name = "测试");

public slots:
    //设置进度条的当前值value, 最大值max
    void setProgress(qint64 valueKB = 0, qint64 maxKB = 100);

private:
    Ui::DataProgress *ui;
};

#endif // DATAPROGRESS_H
