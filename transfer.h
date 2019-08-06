#ifndef TRANSFER_H
#define TRANSFER_H

#include <QWidget>
#include <common/utils.h>

namespace Ui {
class Transfer;
}

class Transfer : public QWidget
{
    Q_OBJECT

public:
    explicit Transfer(QWidget *parent = nullptr);
    ~Transfer();

public:
    void setTable(TransferStatus stat);

private:
    Ui::Transfer *ui;
};

#endif // TRANSFER_H
