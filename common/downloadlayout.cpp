#include "downloadlayout.h"

DownloadLayout &DownloadLayout::getInstance()
{
    static DownloadLayout instance_;
    return instance_;
}

QVBoxLayout *DownloadLayout::getUploadLayout()
{
    return layout_;
}

void DownloadLayout::setUploadLayout(QWidget *pWidget)
{
    QVBoxLayout* vlayout = new QVBoxLayout(pWidget);
    // 布局设置给窗口
    pWidget->setLayout(vlayout);
    // 边界间隔
    vlayout->setContentsMargins(0, 0, 0, 0);
    // 添加弹簧
    vlayout->addStretch();
    layout_ = vlayout;
}

