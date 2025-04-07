#include "addsmartphonedialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

AddSmartphoneDialog::AddSmartphoneDialog(QDialog* parent)
    : QDialog(parent)
{
    nameLineEdit = new QLineEdit(this);
    manufacturerLineEdit = new QLineEdit(this);

    releaseYearLineEdit = new QLineEdit(this);

    displaySizeLineEdit = new QLineEdit(this);
    batterySizeLineEdit = new QLineEdit(this);
    sensorComboBox = new QComboBox(this);
    lensComboBox = new QComboBox(this);
    selectedSensorLensList = new QListWidget(this);

    // Beispielhafte Sensoren und Linsen
    sensors.append(CameraSensor("Sony IMX989", 50 * 1000000, 8000, 6000, 1.1, 7.6, 5.6, 3.5, 4.2, 1.0, 2022, 2.8, "16:9", "1/2,76", "Bayer", 12, "Esel"));

    // UI-Elemente
    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow("Name:", nameLineEdit);
    formLayout->addRow("Hersteller:", manufacturerLineEdit);
    formLayout->addRow("Erscheinungsjahr:", releaseYearLineEdit);
    formLayout->addRow("Displaygröße (Zoll):", displaySizeLineEdit);
    formLayout->addRow("Akkugröße (mAh):", batterySizeLineEdit);
    formLayout->addRow("Sensor auswählen:", sensorComboBox);
    formLayout->addRow("Linse auswählen:", lensComboBox);

    // Sensor- und Lens-ComboBox füllen
    for (const CameraSensor& sensor : sensors) {
        sensorComboBox->addItem(sensor.name());
    }

    for (const Lens& lens : lenses) {
        lensComboBox->addItem(lens.type() + " " + QString::number(lens.apertureMin()) + "mm");
    }

    // Sensor-Linse Paar Liste
    formLayout->addRow("Sensor-Linsen-Paare:", selectedSensorLensList);

    QPushButton* addButton = new QPushButton("Hinzufügen", this);
    QPushButton* okButton = new QPushButton("OK", this);
    QPushButton* cancelButton = new QPushButton("Abbrechen", this);

    connect(addButton, &QPushButton::clicked, this, &AddSmartphoneDialog::addSensorLensPair);
    connect(okButton, &QPushButton::clicked, this, &AddSmartphoneDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Layout zusammenstellen
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(addButton);
    mainLayout->addWidget(okButton);
    mainLayout->addWidget(cancelButton);

    setLayout(mainLayout);
    setWindowTitle("Smartphone hinzufügen");
}

Smartphone AddSmartphoneDialog::getSmartphone() const {
    QString name = nameLineEdit->text();
    QString manufacturer = manufacturerLineEdit->text();
    int releaseYear = releaseYearLineEdit->text().toInt();
    double displaySize = displaySizeLineEdit->text().replace(",", ".").toDouble();
    int batterySize = batterySizeLineEdit->text().toInt();

    Smartphone smartphone(name, manufacturer, releaseYear, displaySize, batterySize);
    for (const QPair<CameraSensor, Lens>& pair : sensorLensPairs) {
        smartphone.addSensorLensPair(pair.first, pair.second);
    }

    return smartphone;
}

void AddSmartphoneDialog::addSensorLensPair() {
    CameraSensor selectedSensor = sensors[sensorComboBox->currentIndex()];
    Lens selectedLens = lenses[lensComboBox->currentIndex()];

    // Sensor-Linsen-Paar zur Liste hinzufügen
    sensorLensPairs.append(QPair<CameraSensor, Lens>(selectedSensor, selectedLens));
    updateSelectedPairs();
}

void AddSmartphoneDialog::updateSelectedPairs() {
    selectedSensorLensList->clear();
    for (const QPair<CameraSensor, Lens>& pair : sensorLensPairs) {
        QString pairText = pair.first.name() + " - " + pair.second.type() + " " + QString::number(pair.second.apertureMin()) + "mm";
        selectedSensorLensList->addItem(pairText);
    }
}

void AddSmartphoneDialog::accept() {
    if (nameLineEdit->text().isEmpty() || manufacturerLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Fehler", "Name und Hersteller müssen angegeben werden.");
        return;
    }

    QDialog::accept();
}
