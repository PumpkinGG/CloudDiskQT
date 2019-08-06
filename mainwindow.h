#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "common/utils.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Invoken by Login
    void showMainWindow();

    // Clear Items Data
    void clearData();

protected:
    void paintEvent(QPaintEvent *event);

private:
    void makeConnects();

signals:
    void sigSwitchUser();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
