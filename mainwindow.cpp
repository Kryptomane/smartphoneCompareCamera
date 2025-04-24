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
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    pLensDialog(new AddLensDialog(this)), // LensDialog initialisieren
    pSensorDialog(new AddSensorDialog(this)) // SensorDialog initialisieren
{
    QTabWidget* tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Compare Tab
    pCompareWidget = new CompareWidget(this);
    pCompareWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pPhoneInfoWidget = new PhoneInfoWidget(this);
    pPhoneInfoWidget->setFixedWidth(200);
    pPhoneInfoWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // Beide Widgets nebeneinander in ein horizontales Layout
    QWidget* compareTab = new QWidget(this);
    QHBoxLayout* hLayout = new QHBoxLayout(compareTab);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(10); // Optional
    // Linke Seite: comparatorWidget + Add-Button
    QVBoxLayout* compareLayout = new QVBoxLayout;
    compareLayout->addWidget(pCompareWidget,0);
    compareLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    QPushButton* addSmartphoneButton = new QPushButton("Add Smartphone", this);
    compareLayout->addWidget(addSmartphoneButton);
    connect(addSmartphoneButton, &QPushButton::clicked, this, &MainWindow::addSmartphone);
    hLayout->addLayout(compareLayout, 3);  // linke Seite – Gewichtung 3
    hLayout->addWidget(pPhoneInfoWidget, 1);
    //ADD Tab
    tabWidget->addTab(compareTab, "Smartphone Compare");

    // Table Tab
    QWidget* tableTab = new QWidget(this);
    // Layout für den Table-Tab
    QVBoxLayout* tableLayout = new QVBoxLayout(tableTab);
    pSensorWidget = new SensorWidget(this);
    pLensWidget = new LensWidget(this);
    pCompareWidget->setSensorAndLensWidgets(pSensorWidget, pLensWidget);
    // Buttons für das Hinzufügen von Sensoren und Linsen
    QPushButton* addSensorButton = new QPushButton("Add CameraSensor", this);
    QPushButton* addLensButton = new QPushButton("Add Lens", this);
    //Elemente
    tableLayout->addWidget(pSensorWidget);
    tableLayout->addWidget(pLensWidget);
    tableLayout->addWidget(addSensorButton);
    tableLayout->addWidget(addLensButton);
    // Buttons verbinden
    connect(addSensorButton, &QPushButton::clicked, this, &MainWindow::addSensor);
    connect(addLensButton, &QPushButton::clicked, this, &MainWindow::addLens);
    connect(this, &MainWindow::printMessage, pCompareWidget, &CompareWidget::printInfoMessage);
    //ADD Tab
    tabWidget->addTab(tableTab, "Camera and Lens");

    // In MainWindow::setupUI() oder ähnlich:
    QWidget* databaseTab = new QWidget;
    QVBoxLayout* dbLayout = new QVBoxLayout(databaseTab);
    QHBoxLayout* pathSelectorLayout = new QHBoxLayout;
    dbPathLineEdit = new QLineEdit;
    dbPathLineEdit->setPlaceholderText(currentDatabaseDirectory);
    QPushButton* browseButton = new QPushButton("Choose Folder");
    pathSelectorLayout->addWidget(dbPathLineEdit);
    pathSelectorLayout->addWidget(browseButton);
    dbLayout->addLayout(pathSelectorLayout);
    dbLayout->addStretch(); // Damit oben bleibt

    // Tab hinzufügen
    tabWidget->addTab(databaseTab, "Databasepath");

    connect(browseButton, &QPushButton::clicked, this, [=]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Choose Database Folder");
        if (!dir.isEmpty()) {
            dbPathLineEdit->setText(dir);
            setDatabaseDirectory(dir);
        }
    });

    defaultDatabasePath();
    loadDatabases();
}

// Destruktor
MainWindow::~MainWindow() {
    exportAllDataToJson();
}

void MainWindow::addSensor() {
    // Öffne den Dialog zum Hinzufügen eines Sensors
    if (pSensorDialog->exec() == QDialog::Accepted) {
        CameraSensor newSensor = pSensorDialog->getSensor(); // Sensor aus Dialog holen
        // Sensor zu einem Smartphone hinzufügen, hier ein Beispiel
        pSensorWidget->addCameraSensor(newSensor);
    }
}

void MainWindow::addLens() {
    // Öffne den Dialog zum Hinzufügen einer Linse
    if (pLensDialog->exec() == QDialog::Accepted) {
        Lens newLens = pLensDialog->getLens(); // Linse aus Dialog holen
        pLensWidget->addLens(newLens);
    }
}

void MainWindow::addSmartphone() {
    AddSmartphoneDialog dialog(this);
    dialog.setSensorAndLensWidgets(pSensorWidget, pLensWidget);
    if (dialog.exec() == QDialog::Accepted) {
        Smartphone newSmartphone = dialog.getSmartphone();
        pCompareWidget->addSmartphone(newSmartphone);
    }
}

LensWidget* MainWindow::getLensesWidget() {
    return pLensWidget;
}

void MainWindow::setDatabaseDirectory(const QString& path) {
    currentDatabaseDirectory = path;
    qDebug() << "Neuer Datenbank-Pfad gesetzt:" << currentDatabaseDirectory;
    reset();
    loadDatabases();
}

void MainWindow::reset(){
    pLensWidget->reset();
    pSensorWidget->reset();
    pCompareWidget->reset();
    pPhoneInfoWidget->reset();
}

void MainWindow::defaultDatabasePath() {
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp(); // raus aus build/Debug
    dir.cdUp(); // raus aus build
    currentDatabaseDirectory = dir.filePath("databases/");
    dbPathLineEdit->setPlaceholderText(currentDatabaseDirectory);
}

QString MainWindow::getDatabasePath(const QString& fileName) {
    QString path = QDir(currentDatabaseDirectory).filePath(fileName);
    if (!QFile::exists(path)) {
        qInfo() << "Datei nicht gefunden:" << path;
        return "";
    } else {
        return path;
    }
}

void MainWindow::exportAllDataToJson() {
    // === 1. Camera Sensors exportieren ===
    QJsonArray sensorsArray;
    for (const CameraSensor& sensor : pSensorWidget->getCameraSensors()) {
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
    for (const Lens& lens : pLensWidget->getLenses()) {
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
    for (const Smartphone& phone : pCompareWidget->getSmartphones()) {
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
        pLensWidget->addLens(lens);
    }
    QString sensorPath = getDatabasePath("camera_sensors.json");
    QJsonArray sensorArray = getJson(sensorPath);
    for (const QJsonValue& value : sensorArray) {
        QJsonObject sensorObject = value.toObject();
        CameraSensor sensor = CameraSensor::fromJson(sensorObject);  // Hier wird angenommen, dass fromJson() existiert
        sensor = CameraSensor::makePlausibelCameraSensor(sensor);
        pSensorWidget->addCameraSensor(sensor);
    }

    QString phonePath = getDatabasePath("smartphones.json");
    QJsonArray smartphoneArray = getJson(phonePath);
    for (const QJsonValue& value : smartphoneArray) {
        QJsonObject phoneObject = value.toObject();
        Smartphone phone = Smartphone::fromJson(phoneObject);  // Hier wird angenommen, dass fromJson() existiert
        pCompareWidget->addSmartphone(phone);
    }

    pPhoneInfoWidget->setSmartphones(pCompareWidget->getSmartphones());
}
