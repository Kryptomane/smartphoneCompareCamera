#ifndef ADDSMARTPHONEDIALOG_H
#define ADDSMARTPHONEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QListWidget>
#include "lens.h"
#include "camerasensor.h"
#include "smartphone.h"
//#include "mainwindow.h"

class AddSmartphoneDialog : public QDialog {
    Q_OBJECT

public:
    // Konstruktor, der das MainWindow übergibt, um darauf zugreifen zu können
    explicit AddSmartphoneDialog(QDialog* parent = nullptr);

    // Gibt das Smartphone zurück, das im Dialog erstellt wurde
    Smartphone getSmartphone() const;

private slots:
    // Slot zum Hinzufügen eines Sensor-Linsen-Paares
    void addSensorLensPair();

    // Überschreibe accept(), um das Smartphone an MainWindow zu übergeben
    void accept() override;

private:
    // Widgets für die Eingabefelder im Dialog
    QLineEdit* nameLineEdit;
    QLineEdit* manufacturerLineEdit;
    QLineEdit* releaseYearLineEdit;
    QLineEdit* displaySizeLineEdit;
    QLineEdit* batterySizeLineEdit;

    // ComboBoxen für die Auswahl von Sensoren und Linsen
    QComboBox* sensorComboBox;
    QComboBox* lensComboBox;

    // Eine Liste der ausgewählten Sensor-Linsen-Paare
    QListWidget* selectedSensorLensList;

    // Listen von Sensoren und Linsen
    QList<CameraSensor> sensors;
    QList<Lens> lenses;

    // Liste von Sensor-Linsen-Paaren
    QList<QPair<CameraSensor, Lens>> sensorLensPairs;

    // Zeiger auf das MainWindow, um auf dessen Methoden und Daten zugreifen zu können
    //MainWindow* m_mainWindow;

    // Aktualisiert die Anzeige der ausgewählten Sensor-Linsen-Paare
    void updateSelectedPairs();
};

#endif // ADDSMARTPHONEDIALOG_H
