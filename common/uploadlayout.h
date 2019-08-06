#ifndef UPLOADLAYOUT_H
#define UPLOADLAYOUT_H

#include <QWidget>
#include <QVBoxLayout>

class UploadLayout
{
public:
    static UploadLayout& getInstance();

    //获取布局
    QVBoxLayout *getUploadLayout();

    //设置布局
    void setUploadLayout(QWidget *pWidget);

private:
    UploadLayout() = default;
    ~UploadLayout() = default;

private:
    QVBoxLayout* layout_;

};

#endif // UPLOADLAYOUT_H
