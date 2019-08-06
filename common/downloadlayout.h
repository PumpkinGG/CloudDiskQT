#ifndef DOWNLOADLAYOUT_H
#define DOWNLOADLAYOUT_H

#include <QWidget>
#include <QVBoxLayout>

class DownloadLayout
{
public:
    static DownloadLayout& getInstance();

    //获取布局
    QVBoxLayout *getUploadLayout();

    //设置布局
    void setUploadLayout(QWidget *pWidget);

private:
    DownloadLayout() = default;
    ~DownloadLayout() = default;

private:
    QVBoxLayout* layout_;

};

#endif // DOWNLOADLAYOUT_H
