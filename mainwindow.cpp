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
    comparatorWidget = new SmartphoneComparatorWidget(this);
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
}

// Destruktor
MainWindow::~MainWindow() {
    saveDataToJson();
}

void MainWindow::saveDataToJson() {
    // Smartphones speichern
    QJsonArray smartphonesJsonArray;
    for (const Smartphone& smartphone : smartphones) {
        smartphonesJsonArray.append(smartphone.toJson());
    }

    QFile smartphonesFile("smartphones.json");
    if (smartphonesFile.open(QIODevice::WriteOnly)) {
        smartphonesFile.write(QJsonDocument(smartphonesJsonArray).toJson());
        smartphonesFile.close();
    }

    // Sensoren und Linsen separat speichern, wenn du zentrale Listen hast
    // (Alternativ: Sensoren/Linsen könnten auch aus den Smartphones rekonstruiert werden)
}

void MainWindow::loadDataFromJson() {
    QFile file("smartphones.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    smartphones.clear();

    for (const QJsonValue& val : doc.array()) {
        smartphones.append(Smartphone::fromJson(val.toObject()));
    }

    // Anschließend ggf. GUI aktualisieren
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
        comparatorWidget->addSmartphones(newSmartphone);
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
    /*
    // Liste von Linsen einlesen
    for (const QJsonValue& value : smartphoneArray) {
        QJsonObject sensorObject = value.toObject();
        Smartphone sensor = Smartphone::fromJson(sensorObject);  // Hier wird angenommen, dass fromJson() existiert
        comparatorWidget->ad
    }
    */
}
