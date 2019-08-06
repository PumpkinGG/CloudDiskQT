#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>

namespace Ui {
class TitleWidget;
}

class TitleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TitleWidget(QWidget *parent = nullptr);
    ~TitleWidget() override;

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void showConfigPage();
    void closeWindow();
    void minimizeWindow();

private:
    Ui::TitleWidget *ui;
    QPoint posDiff_;
    QWidget* parent_;

};

#endif // TITLEWIDGET_H
