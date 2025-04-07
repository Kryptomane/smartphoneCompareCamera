#ifndef ADDLENSDIALOG_H
#define ADDLENSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include "lens.h"

class AddLensDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddLensDialog(QWidget* parent = nullptr);
    Lens getLens() const;

private:
    mutable Lens createdLens;

    QLineEdit* idEdit;
    QLineEdit* focalMinEdit;
    QLineEdit* focalMaxEdit;
    QLineEdit* apertureMinEdit;
    QLineEdit* apertureMaxEdit;
};

#endif // ADDLENSDIALOG_H
