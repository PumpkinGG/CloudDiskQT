#ifndef FILEPROPERTY_H
#define FILEPROPERTY_H

#include <QDialog>
#include "common/utils.h"

namespace Ui {
class FileProperty;
}

class FileProperty : public QDialog
{
    Q_OBJECT

public:
    explicit FileProperty(QWidget *parent = nullptr);
    ~FileProperty();

public slots:
    void setFileInfo(FileInfo* fileInfo);

private:
    Ui::FileProperty *ui;
};

#endif // FILEPROPERTY_H
