#ifndef ADDSMARTPHONEDIALOG_H
#define ADDSMARTPHONEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QListWidget>
#include <QMessageBox>

#include "lens.h"
#include "camerasensor.h"
#include "smartphone.h"
#include "camerasensortablewidget.h"
#include "lenstablewidget.h"

class AddSmartphoneDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddSmartphoneDialog(QWidget* parent = nullptr);

    Smartphone getSmartphone() const;
    void setSensorAndLensWidgets(CameraSensorTableWidget* sensorWidget, LensTableWidget* lensWidget);
    void reset();  // Felder zurücksetzen

private slots:
    void addSensorLensPair();
    void accept() override;

private:
    QLineEdit* nameLineEdit;
    QLineEdit* manufacturerLineEdit;
    QLineEdit* releaseYearLineEdit;
    QLineEdit* displaySizeLineEdit;
    QLineEdit* batterySizeLineEdit;

    QComboBox* sensorComboBox;
    QComboBox* lensComboBox;
    QListWidget* selectedSensorLensList;

    QList<QPair<CameraSensor, Lens>> sensorLensPairs;

    CameraSensorTableWidget* cameraSensorTableWidget = nullptr;
    LensTableWidget* lensTableWidget = nullptr;

    void updateSelectedPairs();
    double calculateFieldOfView(double sensorWidthMM, double focalLengthMM) const;
    double calculateAngularResolution(double resolutionWidth, double fovDegrees) const;
};

#endif // ADDSMARTPHONEDIALOG_H
