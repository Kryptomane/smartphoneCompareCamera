#include "addsmartphonedialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QtMath>

AddSmartphoneDialog::AddSmartphoneDialog(QWidget* parent)
    : QDialog(parent) {

    setWindowTitle("Smartphone hinzufügen");

    nameLineEdit = new QLineEdit;
    manufacturerLineEdit = new QLineEdit;
    releaseYearLineEdit = new QLineEdit;
    displaySizeLineEdit = new QLineEdit;
    batterySizeLineEdit = new QLineEdit;

    sensorComboBox = new QComboBox;
    lensComboBox = new QComboBox;
    selectedSensorLensList = new QListWidget;

    QPushButton* addPairButton = new QPushButton("Sensor-Linse hinzufügen");
    QPushButton* okButton = new QPushButton("OK");
    QPushButton* cancelButton = new QPushButton("Abbrechen");

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow("Name:", nameLineEdit);
    formLayout->addRow("Hersteller:", manufacturerLineEdit);
    formLayout->addRow("Release-Jahr:", releaseYearLineEdit);
    formLayout->addRow("Displaygröße (Zoll):", displaySizeLineEdit);
    formLayout->addRow("Akkukapazität (mAh):", batterySizeLineEdit);
    formLayout->addRow("Sensor auswählen:", sensorComboBox);
    formLayout->addRow("Linse auswählen:", lensComboBox);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(addPairButton);
    mainLayout->addWidget(new QLabel("Ausgewählte Sensor-Linsen-Paare:"));
    mainLayout->addWidget(selectedSensorLensList);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(addPairButton, &QPushButton::clicked, this, &AddSmartphoneDialog::addSensorLensPair);
    connect(okButton, &QPushButton::clicked, this, &AddSmartphoneDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void AddSmartphoneDialog::setSensorAndLensWidgets(CameraSensorTableWidget* sensorWidget, LensTableWidget* lensWidget) {
    cameraSensorTableWidget = sensorWidget;
    lensTableWidget = lensWidget;

    // ComboBoxen befüllen
    sensorComboBox->clear();
    lensComboBox->clear();

    const QList<CameraSensor>& sensors = sensorWidget->getCameraSensors();
    const QList<Lens>& lenses = lensWidget->getLenses();

    for (const auto& sensor : sensors)
        sensorComboBox->addItem(sensor.name());

    for (const auto& lens : lenses)
        lensComboBox->addItem(lens.id());
}

void AddSmartphoneDialog::addSensorLensPair() {
    if (!cameraSensorTableWidget || !lensTableWidget)
        return;

    int sensorIndex = sensorComboBox->currentIndex();
    int lensIndex = lensComboBox->currentIndex();

    if (sensorIndex < 0 || lensIndex < 0)
        return;

    const CameraSensor& sensor = cameraSensorTableWidget->getCameraSensors().at(sensorIndex);
    const Lens& lens = lensTableWidget->getLenses().at(lensIndex);

    sensorLensPairs.append(qMakePair(sensor, lens));
    updateSelectedPairs();
}

void AddSmartphoneDialog::updateSelectedPairs() {
    selectedSensorLensList->clear();
    for (const auto& pair : sensorLensPairs) {
        QString entry = QString("%1 + %2").arg(pair.first.name(), pair.second.id());
        selectedSensorLensList->addItem(entry);
    }
}

Smartphone AddSmartphoneDialog::getSmartphone() const {
    Smartphone phone;
    phone.setName(nameLineEdit->text());
    phone.setManufacturer(manufacturerLineEdit->text());
    phone.setReleaseYear(releaseYearLineEdit->text().toInt());
    phone.setDisplaySize(displaySizeLineEdit->text().toDouble());
    phone.setBatteryCapacity(batterySizeLineEdit->text().toInt());

    for (const auto& pair : sensorLensPairs) {
        const CameraSensor& sensor = pair.first;
        const Lens& lens = pair.second;

        phone.addSensorLensPair(sensor, lens);

        // Berechnung: Öffnungswinkel (FOV)
        double fov = calculateFieldOfView(sensor.width(), lens.focalLengthMin());
        phone.addFieldOfView(fov);

        // Berechnung: Winkelauflösung (Pixel per Degree)
        double pxd = calculateAngularResolution(sensor.horizontalPixels(), fov);
        phone.addAngularResolution(pxd);
    }

    return phone;
}

void AddSmartphoneDialog::accept() {
    if (nameLineEdit->text().isEmpty() || sensorLensPairs.isEmpty()) {
        QMessageBox::warning(this, "Fehler", "Bitte geben Sie alle Pflichtfelder an und fügen Sie mindestens ein Sensor-Linsen-Paar hinzu.");
        return;
    }

    QDialog::accept();
}

void AddSmartphoneDialog::reset() {
    nameLineEdit->clear();
    manufacturerLineEdit->clear();
    releaseYearLineEdit->clear();
    displaySizeLineEdit->clear();
    batterySizeLineEdit->clear();
    sensorLensPairs.clear();
    selectedSensorLensList->clear();
}

double AddSmartphoneDialog::calculateFieldOfView(double sensorWidthMM, double focalLengthMM) const {
    return 2.0 * qRadiansToDegrees(qAtan(sensorWidthMM / (2.0 * focalLengthMM)));
}

double AddSmartphoneDialog::calculateAngularResolution(double resolutionWidth, double fovDegrees) const {
    return resolutionWidth / fovDegrees;
}
