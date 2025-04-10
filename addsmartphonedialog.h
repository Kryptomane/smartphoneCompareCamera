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
    void addMainCam();
    void addSelfieCam();
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

    QList<SensorLensPair> mainCams;
    QList<SensorLensPair> selfieCams;

    CameraSensorTableWidget* cameraSensorTableWidget = nullptr;
    LensTableWidget* lensTableWidget = nullptr;

    void updateSelectedPairs();
    double calculateFieldOfView(double sensorDiagonalMM, double cropFactor, double equivalentFocalLengthMM);
};

#endif // ADDSMARTPHONEDIALOG_H
