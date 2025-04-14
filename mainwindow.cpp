#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QFile>
#include <QLabel>
#include "lens.h"
#include "camerasensor.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    lensDialog(new AddLensDialog(this)), // LensDialog initialisieren
    sensorDialog(new AddSensorDialog(this)) // SensorDialog initialisieren
{
    QTabWidget* tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Compare Tab
    comparatorWidget = new phoneCompareWidget(this);

    // Smartphone hinzufügen Button
    QPushButton* addSmartphoneButton = new QPushButton("Smartphone hinzufügen", this);
    // Layout des comparatorWidgets erweitern
    QVBoxLayout* compareLayout = qobject_cast<QVBoxLayout*>(comparatorWidget->layout());
    if (compareLayout) {
        compareLayout->addWidget(addSmartphoneButton);
    }
    connect(addSmartphoneButton, &QPushButton::clicked, this, &MainWindow::addSmartphone);
    tabWidget->addTab(comparatorWidget, "Smartphone Compare");

    // Table Tab
    QWidget* tableTab = new QWidget(this);
    tabWidget->addTab(tableTab, "Camera and Lens");

    // Layout für den Table-Tab
    QVBoxLayout* tableLayout = new QVBoxLayout(tableTab);
    cameraSensorTableWidget = new CameraSensorTableWidget(this);
    lensTableWidget = new LensTableWidget(this);
    comparatorWidget->setSensorAndLensWidgets(cameraSensorTableWidget, lensTableWidget);

    // Buttons für das Hinzufügen von Sensoren und Linsen
    QPushButton* addSensorButton = new QPushButton("Sensor hinzufügen", this);
    QPushButton* addLensButton = new QPushButton("Linse hinzufügen", this);

    tableLayout->addWidget(cameraSensorTableWidget);
    tableLayout->addWidget(lensTableWidget);
    tableLayout->addWidget(addSensorButton);
    tableLayout->addWidget(addLensButton);

    // Buttons verbinden
    connect(addSensorButton, &QPushButton::clicked, this, &MainWindow::addSensor);
    connect(addLensButton, &QPushButton::clicked, this, &MainWindow::addLens);
    //loadData
    loadLensData();
    loadCameraSensors();
    //loadSmartPhones();
}

// Destruktor
MainWindow::~MainWindow() {
    //saveDataToJson();
    exportAllDataToJson();

}

void MainWindow::exportAllDataToJson() {
    // === 1. Camera Sensors exportieren ===
    QJsonArray sensorsArray;
    for (const CameraSensor& sensor : cameraSensorTableWidget->getCameraSensors()) {
        sensorsArray.append(sensor.toJson());
    }
    QJsonDocument sensorsDoc(sensorsArray);
    QFile sensorsFile("camera_sensors.json");
    if (sensorsFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        sensorsFile.write(sensorsDoc.toJson(QJsonDocument::Indented));
        sensorsFile.close();
    } else {
        qWarning() << "Konnte camera_sensors.json nicht schreiben.";
    }

    // === 2. Lenses exportieren ===
    QJsonArray lensesArray;
    for (const Lens& lens : lensTableWidget->getLenses()) {
        lensesArray.append(lens.toJson());
    }
    QJsonDocument lensesDoc(lensesArray);
    QFile lensesFile("lenses.json");
    if (lensesFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        lensesFile.write(lensesDoc.toJson(QJsonDocument::Indented));
        lensesFile.close();
    } else {
        qWarning() << "Konnte lenses.json nicht schreiben.";
    }

    // === 3. Smartphones exportieren ===
    QJsonArray smartphonesArray;
    for (const Smartphone& phone : comparatorWidget->getSmartphones()) {
        smartphonesArray.append(phone.toJson());
    }
    QJsonDocument smartphonesDoc(smartphonesArray);
    QFile smartphonesFile("smartphones.json");
    if (smartphonesFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        smartphonesFile.write(smartphonesDoc.toJson(QJsonDocument::Indented));
        smartphonesFile.close();
    } else {
        qWarning() << "Konnte smartphones.json nicht schreiben.";
    }
}

void MainWindow::addSensor() {
    // Öffne den Dialog zum Hinzufügen eines Sensors
    if (sensorDialog->exec() == QDialog::Accepted) {
        CameraSensor newSensor = sensorDialog->getSensor(); // Sensor aus Dialog holen
        // Sensor zu einem Smartphone hinzufügen, hier ein Beispiel
        cameraSensorTableWidget->addCameraSensor(newSensor);
    }
}

void MainWindow::addLens() {
    // Öffne den Dialog zum Hinzufügen einer Linse
    if (lensDialog->exec() == QDialog::Accepted) {
        Lens newLens = lensDialog->getLens(); // Linse aus Dialog holen
        lensTableWidget->addLens(newLens);
    }
}

void MainWindow::addSmartphone() {
    AddSmartphoneDialog dialog(this);
    dialog.setSensorAndLensWidgets(cameraSensorTableWidget, lensTableWidget);
    if (dialog.exec() == QDialog::Accepted) {
        Smartphone newSmartphone = dialog.getSmartphone();
        comparatorWidget->addSmartphone(newSmartphone);
    }
}

LensTableWidget* MainWindow::getLensesWidget() {
    return lensTableWidget;
}

void MainWindow::loadLensData()
{
    // Pfad zur JSON-Datei
    QString filePath = "lens.json";  // Ersetze durch den tatsächlichen Pfad

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Fehler", "Konnte die JSON-Datei nicht öffnen.");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    // JSON-Dokument erstellen
    QJsonDocument document = QJsonDocument::fromJson(fileData);
    if (document.isNull()) {
        QMessageBox::warning(this, "Fehler", "Ungültiges JSON-Format.");
        return;
    }

    QJsonArray lensArray = document.array();  // JSON-Array der Linsen

    // Liste von Linsen einlesen
    for (const QJsonValue& value : lensArray) {
        QJsonObject lensObject = value.toObject();
        Lens lens = Lens::fromJson(lensObject);  // Hier wird angenommen, dass fromJson() existiert
        lensTableWidget->addLens(lens);
    }
}

void MainWindow::loadCameraSensors()
{
    // Pfad zur JSON-Datei
    QString filePath = "camerasensors.json";  // Ersetze durch den tatsächlichen Pfad

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Fehler", "Konnte die JSON-Datei nicht öffnen.");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    // JSON-Dokument erstellen
    QJsonDocument document = QJsonDocument::fromJson(fileData);
    if (document.isNull()) {
        QMessageBox::warning(this, "Fehler", "Ungültiges JSON-Format.");
        return;
    }

    QJsonArray sensorArray = document.array();  // JSON-Array der Linsen

    // Liste von Linsen einlesen
    for (const QJsonValue& value : sensorArray) {
        QJsonObject sensorObject = value.toObject();
        CameraSensor sensor = CameraSensor::fromJson(sensorObject);  // Hier wird angenommen, dass fromJson() existiert
        sensor = CameraSensor::makePlausibelCameraSensor(sensor);
        cameraSensorTableWidget->addCameraSensor(sensor);
    }
}

void MainWindow::loadSmartPhones()
{
    // Pfad zur JSON-Datei
    QString filePath = "smartphones.json";  // Ersetze durch den tatsächlichen Pfad

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Fehler", "Konnte die JSON-Datei nicht öffnen.");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    // JSON-Dokument erstellen
    QJsonDocument document = QJsonDocument::fromJson(fileData);
    if (document.isNull()) {
        QMessageBox::warning(this, "Fehler", "Ungültiges JSON-Format.");
        return;
    }

    QJsonArray smartphoneArray = document.array();  // JSON-Array der Linsen

    // Liste von Linsen einlesen
    for (const QJsonValue& value : smartphoneArray) {
        QJsonObject phoneObject = value.toObject();
        Smartphone phone = Smartphone::fromJson(phoneObject);  // Hier wird angenommen, dass fromJson() existiert
        comparatorWidget->addSmartphone(phone);
    }
}
