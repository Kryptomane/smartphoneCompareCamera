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

    QPushButton* addMainCamButton = new QPushButton("Main Camera hinzufügen");
    QPushButton* addSelfieCamButton = new QPushButton("Selfie Camera hinzufügen");
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
    mainLayout->addWidget(addMainCamButton);
    mainLayout->addWidget(addSelfieCamButton);
    mainLayout->addWidget(new QLabel("Ausgewählte Sensor-Linsen-Paare:"));
    mainLayout->addWidget(selectedSensorLensList);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(addMainCamButton, &QPushButton::clicked, this, &AddSmartphoneDialog::addMainCam);
    connect(addSelfieCamButton, &QPushButton::clicked, this, &AddSmartphoneDialog::addSelfieCam);
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
    {
        QString temp = QString(lens.type()+" %1mm, f=%2").arg(QString::number(lens.focalLengthMin()), QString::number(lens.apertureMin()));
        lensComboBox->addItem(temp);
    }
}

void AddSmartphoneDialog::addMainCam() {
    if (!cameraSensorTableWidget || !lensTableWidget)
        return;

    int sensorIndex = sensorComboBox->currentIndex();
    int lensIndex = lensComboBox->currentIndex();

    if (sensorIndex < 0 || lensIndex < 0)
        return;

    const CameraSensor& sensor = cameraSensorTableWidget->getCameraSensors().at(sensorIndex);
    const Lens& lens = lensTableWidget->getLenses().at(lensIndex);

    SensorLensPair pair(sensor.name(),lens.id(),calculateFieldOfView(sensor.diagonal(), sensor.cropFactor(), lens.apertureMin()));

    mainCams.append(pair);
    updateSelectedPairs();
}

void AddSmartphoneDialog::addSelfieCam() {
    if (!cameraSensorTableWidget || !lensTableWidget)
        return;

    int sensorIndex = sensorComboBox->currentIndex();
    int lensIndex = lensComboBox->currentIndex();

    if (sensorIndex < 0 || lensIndex < 0)
        return;

    const CameraSensor& sensor = cameraSensorTableWidget->getCameraSensors().at(sensorIndex);
    const Lens& lens = lensTableWidget->getLenses().at(lensIndex);

    SensorLensPair pair(sensor.name(),lens.id(),calculateFieldOfView(sensor.diagonal(), sensor.cropFactor(), lens.apertureMin()));

    selfieCams.append(pair);
    updateSelectedPairs();
}

void AddSmartphoneDialog::updateSelectedPairs() {
    selectedSensorLensList->clear();
    for (const auto& pair : mainCams) {
        QString entry = QString("%1 + %2 (FOV: %3°)")
                            .arg(pair.sensorName, pair.lensId)
                            .arg(QString::number(pair.fieldOfView, 'f', 2));  // Zeigt den FOV auf 2 Dezimalstellen an
        selectedSensorLensList->addItem(entry);
    }
    for (const auto& pair : selfieCams) {
        QString entry = QString("%1 + %2 (FOV: %3°)")
                            .arg(pair.sensorName, pair.lensId)
                            .arg(QString::number(pair.fieldOfView, 'f', 2));  // Zeigt den FOV auf 2 Dezimalstellen an
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

    // Sensor-Linsen-Paare aus mainCams hinzufügen
    for (const auto& pair : mainCams) {
        // Einfach das Paar mit dem bereits vorhandenen Öffnungswinkel (FOV) hinzufügen
        phone.addMainCam(pair);
    }

    // Optionale Behandlung für Selfie-Kameras, falls benötigt
    for (const auto& pair : selfieCams) {
        phone.addSelfieCam(pair);
    }

    return phone;
}

void AddSmartphoneDialog::accept() {
    if (nameLineEdit->text().isEmpty() || mainCams.isEmpty()) {
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
    mainCams.clear();
    selfieCams.clear();
    selectedSensorLensList->clear();
}

// Berechnet den Öffnungswinkel (Field of View) anhand der Sensordiagonale, des Crop-Faktors und der KB-äquivalenten Brennweite
double AddSmartphoneDialog::calculateFieldOfView(double sensorDiagonal, double cropFactor, double kbFocalLength) {
    // Umrechnung der KB-äquivalenten Brennweite in die tatsächliche Brennweite des Sensors
    double fl = kbFocalLength / cropFactor;
    // Berechnung des horizontalen Sichtfelds (FOV) in Grad mit der Formel:
    // FOV = 2 * atan(sensorDiagonal / (2 * Brennweite))
    return 2.0 * qRadiansToDegrees(qAtan(sensorDiagonal / (2.0 * fl)));
}
