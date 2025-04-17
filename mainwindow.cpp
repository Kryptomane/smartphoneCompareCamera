#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QFile>
#include <QLabel>
#include "lens.h"
#include "camerasensor.h"
#include <QMessageBox>
#include <qcoreapplication.h>
#include <QDir>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    lensDialog(new AddLensDialog(this)), // LensDialog initialisieren
    sensorDialog(new AddSensorDialog(this)) // SensorDialog initialisieren
{
    QTabWidget* tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Compare Tab
    comparatorWidget = new phoneCompareWidget(this);
    comparatorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    phoneDetailWidget* smartphoneDetailsWidget = new phoneDetailWidget(this);
    smartphoneDetailsWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    // Beide Widgets nebeneinander in ein horizontales Layout
    QWidget* compareContainer = new QWidget(this);
    QHBoxLayout* hLayout = new QHBoxLayout(compareContainer);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(10); // Optional
    // Linke Seite: comparatorWidget + Add-Button
    QVBoxLayout* compareLayout = new QVBoxLayout;
    compareLayout->addWidget(comparatorWidget,0);

    QPushButton* addSmartphoneButton = new QPushButton("Smartphone hinzufügen", this);
    compareLayout->addWidget(addSmartphoneButton);
    connect(addSmartphoneButton, &QPushButton::clicked, this, &MainWindow::addSmartphone);

    hLayout->addLayout(compareLayout, 3);  // linke Seite – Gewichtung 3
    hLayout->addWidget(smartphoneDetailsWidget, 1); // rechte Seite – Gewichtung 2

    tabWidget->addTab(compareContainer, "Smartphone Compare");

    // Nach dem Laden der Smartphones:


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
    connect(this, &MainWindow::printMessage, comparatorWidget, &phoneCompareWidget::printInfoMessage);

    loadDatabases();

    smartphoneDetailsWidget->setSmartphones(comparatorWidget->getSmartphones());
}

// Destruktor
MainWindow::~MainWindow() {
    exportAllDataToJson();
}

QString MainWindow::getDatabasePath(const QString& fileName) {
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp(); // raus aus build/Debug
    dir.cdUp(); // raus aus build
    QString path = dir.filePath("databases/" + fileName);
    if (!QFile::exists(path)) {
        qInfo() << "Datei nicht gefunden:" << path;
    } else {
        return path;
    }
}

void MainWindow::exportAllDataToJson() {
    // === 1. Camera Sensors exportieren ===
    QJsonArray sensorsArray;
    for (const CameraSensor& sensor : cameraSensorTableWidget->getCameraSensors()) {
        sensorsArray.append(sensor.toJson());
    }
    QJsonDocument sensorsDoc(sensorsArray);
    QString sensorPath = getDatabasePath("camera_sensors.json");
    QFile sensorsFile(sensorPath);
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
    QString lensesPath = getDatabasePath("lenses.json");
    QFile lensesFile(lensesPath);
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
    QString phonePath = getDatabasePath("smartphones.json");
    QFile smartphonesFile(phonePath);
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

QJsonArray MainWindow::getJson(QString filePath){
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Fehler", "Konnte die JSON-Datei nicht öffnen.");
        return QJsonArray();
    }

    QByteArray fileData = file.readAll();
    file.close();

    // JSON-Dokument erstellen
    QJsonDocument document = QJsonDocument::fromJson(fileData);
    if (document.isNull()) {
        QMessageBox::warning(this, "Fehler", "Ungültiges JSON-Format.");
        return QJsonArray();
    }

    QJsonArray Array = document.array();  // JSON-Array der Linsen
    return Array;
}

void MainWindow::loadDatabases(){
    QString lensPath = getDatabasePath("lenses.json");
    QJsonArray lensArray = getJson(lensPath);
    // Liste von Linsen einlesen
    for (const QJsonValue& value : lensArray) {
        QJsonObject lensObject = value.toObject();
        Lens lens = Lens::fromJson(lensObject);  // Hier wird angenommen, dass fromJson() existiert
        lensTableWidget->addLens(lens);
    }
    QString sensorPath = getDatabasePath("camera_sensors.json");
    QJsonArray sensorArray = getJson(sensorPath);
    for (const QJsonValue& value : sensorArray) {
        QJsonObject sensorObject = value.toObject();
        CameraSensor sensor = CameraSensor::fromJson(sensorObject);  // Hier wird angenommen, dass fromJson() existiert
        sensor = CameraSensor::makePlausibelCameraSensor(sensor);
        cameraSensorTableWidget->addCameraSensor(sensor);
    }

    QString phonePath = getDatabasePath("smartphones.json");
    QJsonArray smartphoneArray = getJson(phonePath);
    for (const QJsonValue& value : smartphoneArray) {
        QJsonObject phoneObject = value.toObject();
        Smartphone phone = Smartphone::fromJson(phoneObject);  // Hier wird angenommen, dass fromJson() existiert
        comparatorWidget->addSmartphone(phone);
    }
}
