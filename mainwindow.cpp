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
        if (!smartphones.isEmpty()) {
            smartphones.first().addSensorLensPair(newSensor, Lens()); // Beispiel für Hinzufügen
        }
        comparatorWidget->updateComparisonTable(); // Tabelle aktualisieren
        cameraSensorTableWidget->addCameraSensor(newSensor);
    }
}

void MainWindow::addLens() {
    // Öffne den Dialog zum Hinzufügen einer Linse
    if (lensDialog->exec() == QDialog::Accepted) {
        Lens newLens = lensDialog->getLens(); // Linse aus Dialog holen
        // Linse zu einem Smartphone hinzufügen, hier ein Beispiel
        if (!smartphones.isEmpty()) {
            smartphones.first().addSensorLensPair(CameraSensor(), newLens); // Beispiel für Hinzufügen
        }
        comparatorWidget->updateComparisonTable(); // Tabelle aktualisieren
        lensTableWidget->addLens(newLens);
    }
}

void MainWindow::addSmartphone() {
    // Dialog erzeugen
    AddSmartphoneDialog dialog(this);

    // Sensor- und Linsen-Widgets übergeben
    dialog.setSensorAndLensWidgets(cameraSensorTableWidget, lensTableWidget);

    // Dialog anzeigen und bei Bestätigung Smartphone übernehmen
    if (dialog.exec() == QDialog::Accepted) {
        Smartphone newSmartphone = dialog.getSmartphone();
        smartphones.append(newSmartphone);
        comparatorWidget->setSmartphones(smartphones);
        comparatorWidget->updateComparisonTable();
    }
}


LensTableWidget* MainWindow::getLensesWidget() {
    return lensTableWidget;
}

void MainWindow::updateSensorAndLensLists() {
    sensorListWidget->clear();
    lensListWidget->clear();

    // Iterieren durch die Smartphones und alle Sensoren und Linsen sammeln
    for (const Smartphone& phone : smartphones) {
        for (const auto& pair : phone.getSensorLensPairs()) {
            const CameraSensor& sensor = pair.first;
            const Lens& lens = pair.second;

            // Kamera-Sensor in die Liste einfügen
            sensorListWidget->addItem(sensor.name() + " - " + QString::number(sensor.resolution() / 1e6) + "MP");

            // Linse in die Liste einfügen
            lensListWidget->addItem(lens.id() + " - " + QString::number(lens.focalLengthMin()) + "mm");
        }
    }
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

void MainWindow::addTestSmartphones() {
    // Zwei Smartphones mit je 3 Kameras und Objektiven hinzufügen
    /*
    // Smartphone 1
    Smartphone phone1("Smartphone 1", "Hersteller 1", 2023, 6.5, 4000);
    CameraSensor sensor1("Sensor 1", 12 * 1000000, 4000, 3000, 1.12, 7.5, 50.0, 5.6, 4.5, 1.0, 2021, 3.0, "4:3", "Sonstiges");
    Lens lens1(1, "Festbrennweite", 15, 15, 2.0, 2.0);
    phone1.addSensorLensPair(sensor1, lens1);

    CameraSensor sensor2("Sensor 2", 12 * 1000000, 4000, 3000, 1.08, 6.0, 50.0, 5.5, 4.2, 1.0, 2022, 3.5, "16:9", "Sonstiges");
    Lens lens2(2, "Festbrennweite", 24, 24, 1.6, 1.6);
    phone1.addSensorLensPair(sensor2, lens2);

    CameraSensor sensor3("Sensor 3", 12 * 1000000, 4000, 3000, 1.0, 5.0, 50.0, 4.5, 4.0, 1.0, 2022, 2.8, "4:3", "Sonstiges");
    Lens lens3(3, "Festbrennweite", 85, 85, 2.5, 2.5);
    phone1.addSensorLensPair(sensor3, lens3);

    smartphones.append(phone1);

    // Smartphone 2
    Smartphone phone2("Smartphone 2", "Hersteller 2", 2023, 6.5, 4000);
    CameraSensor sensor4("Sensor 1", 8192, 1.12, "4:3", 2024);
    Lens lens4(1, "Festbrennweite", 12, 12, 1.8, 1.8);
    phone2.addSensorLensPair(sensor4, lens4);

    CameraSensor sensor5("Sensor 2", 8192, 1.08, "16:9", 2023);
    Lens lens5(2, "Festbrennweite", 23, 23, 1.8, 1.8);
    phone2.addSensorLensPair(sensor5, lens5);

    CameraSensor sensor6("Sensor 3", 8192, 1.0, "4:3", 2024);
    Lens lens6(3, "Festbrennweite", 105, 105, 3.5, 3.5);
    phone2.addSensorLensPair(sensor6, lens6);
    smartphones.append(phone2);

    Smartphone phone3("Smartphone 3", "Hersteller 3", 2023, 6.5, 4000);
    CameraSensor sensor7("Sensor 7", 819, 1.12, "4:3", 2024);
    Lens lens7(1, "Festbrennweite", 12, 12, 1.8, 1.8);
    phone1.addSensorLensPair(sensor1, lens1);

    CameraSensor sensor8("Sensor 8", 819, 1.08, "16:9", 2023);
    Lens lens8(2, "Festbrennweite", 23, 23, 1.8, 1.8);
    phone1.addSensorLensPair(sensor2, lens2);

    CameraSensor sensor9("Sensor 9", 819, 1.0, "4:3", 2024);
    Lens lens9(3, "Festbrennweite", 105, 105, 3.5, 3.5);
    phone1.addSensorLensPair(sensor3, lens3);
    smartphones.append(phone3);
    */
}


