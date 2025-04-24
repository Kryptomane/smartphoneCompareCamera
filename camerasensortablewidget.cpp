#include "camerasensortablewidget.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QBrush>

SensorWidget::SensorWidget(QWidget *parent)
    : QWidget(parent),
    sensorTable(new QTableWidget(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(sensorTable);
    sensorTable->setColumnCount(10);  // 6 Spalten: id, type, pixelCount, sensorSize, resolution, sensorType
    sensorTable->setHorizontalHeaderLabels({"Manufacture", "Name", "InchSize", "Area", "Resolution", "Format",
                                            "HorizontalPixels", "VerticalPixels", "PixelSize", "Diagonal" });
    sensorTable->horizontalHeader()->setStretchLastSection(true);
}

void SensorWidget::setCameraSensors(const QList<CameraSensor>& sensors)
{
    cameraSensorsList = sensors;
    updateTable();
}

void SensorWidget::updateTable()
{
    sensorTable->setRowCount(cameraSensorsList.size());  // Anzahl der Zeilen entspricht der Anzahl der Kamera-Sensoren

    for (int row = 0; row < cameraSensorsList.size(); ++row)
    {
        const CameraSensor& sensor = cameraSensorsList[row];
        sensorTable->setItem(row, 0, new QTableWidgetItem(sensor.manufacture()));
        sensorTable->setItem(row, 1, new QTableWidgetItem(sensor.name()));
        sensorTable->setItem(row, 2, new QTableWidgetItem(sensor.inchSize()));
        sensorTable->setItem(row, 3, new QTableWidgetItem(QString::number(sensor.sensorArea())));
        sensorTable->setItem(row, 4, new QTableWidgetItem(QString::number(sensor.resolution())));
        sensorTable->setItem(row, 5, new QTableWidgetItem(sensor.format()));
        sensorTable->setItem(row, 6, new QTableWidgetItem(QString::number(sensor.horizontalPixels())));
        sensorTable->setItem(row, 7, new QTableWidgetItem(QString::number(sensor.verticalPixels())));
        sensorTable->setItem(row, 8, new QTableWidgetItem(QString::number(sensor.pixelSize())));
        sensorTable->setItem(row, 9, new QTableWidgetItem(QString::number(sensor.diagonal())));
    }
}

QList<CameraSensor> SensorWidget::getCameraSensors() const
{
    return cameraSensorsList;  // Gibt die interne Liste der Kamera-Sensoren zurück
}

CameraSensor SensorWidget::getCameraByName(const QString& cameraName) const {
    for (const CameraSensor& sens : cameraSensorsList) {
        if (sens.name() == cameraName) {
            return sens;
        }
    }
    // Optional: Fehlerbehandlung oder Dummy-Lens zurückgeben
    qWarning() << "CameraSensor with Name" << cameraName << "not found!";
    return CameraSensor(); // Leere/dummy Lens zurückgeben
}

void SensorWidget::addCameraSensor(const CameraSensor& sensor)
{
    cameraSensorsList.append(sensor);  // Füge den neuen Kamera-Sensor zur Liste hinzu
    updateTable();  // Aktualisiere die Tabelle
}

void SensorWidget::reset(){
    cameraSensorsList.clear();
    updateTable();
}
