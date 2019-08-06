#include "uploadlayout.h"

UploadLayout &UploadLayout::getInstance()
{
    static UploadLayout instance_;
    return instance_;
}

QVBoxLayout *UploadLayout::getUploadLayout()
{
    return layout_;
}

void UploadLayout::setUploadLayout(QWidget *pWidget)
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
