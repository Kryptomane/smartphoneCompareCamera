#ifndef ADDSENSORDIALOG_H
#define ADDSENSORDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include "camerasensor.h"

class AddSensorDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddSensorDialog(QWidget* parent = nullptr);
    CameraSensor getSensor() const;

private:
    mutable CameraSensor createdSensor;

    QLineEdit* nameEdit;
    QLineEdit* pixelsEdit;
    QLineEdit* pixelSizeEdit;
    QLineEdit* formatEdit;
    QLineEdit* yearEdit;
    QLineEdit* inchSizeEdit;
    QLineEdit* resolutionEdit;
    QComboBox* formatComboBox;
};

#endif // ADDSENSORDIALOG_H
