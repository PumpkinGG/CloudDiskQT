#ifndef BUTTONGROUP_H
#define BUTTONGROUP_H

#include <QWidget>
#include <QSignalMapper>
#include <QMap>
#include "common/utils.h"

namespace Ui {
class ButtonGroup;
}

class QToolButton;

class ButtonGroup : public QWidget
{
    Q_OBJECT

public:
    explicit ButtonGroup(QWidget *parent = nullptr);
    ~ButtonGroup();

public:
    void setParent(QWidget *parent);

public slots:
    // 按钮处理函数
    void slotButtonClick(QString text);
    // 用于处理非按钮触发
    void slotButtonClick(MainPages page);

    void setUserName(QString name);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent* event);

signals:
    void sigMyFile();
    void sigShareList();
    void sigRanking();
    void sigTransfer();
    void sigSwitchUser();
    void closeWindow();
    void minWindow();
    void maxWindow();
    void normalWindow();

private:
    Ui::ButtonGroup *ui;

    QPoint posDiff_;
    QWidget* parent_;
    QToolButton* curBtn_;

    QMap<MainPages, QString> btnTextMap_;
    QMap<QString, QToolButton*> btns_;

};

#endif // BUTTONGROUP_H
