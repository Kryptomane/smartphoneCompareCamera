#include "camerasensortablewidget.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QBrush>

CameraSensorTableWidget::CameraSensorTableWidget(QWidget *parent)
    : QWidget(parent),
    sensorTable(new QTableWidget(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(sensorTable);
    sensorTable->setColumnCount(12);  // 6 Spalten: id, type, pixelCount, sensorSize, resolution, sensorType
    sensorTable->setHorizontalHeaderLabels({"Name", "PixelCount", "HorizontalPixels", "VerticalPixels", "PixelSize", "SensorSize",
                                            "Width", "Height", "Diagonal", "CropFactor", "ReleaseYear", "Sensitivity", "Format", "Other"});
    sensorTable->horizontalHeader()->setStretchLastSection(true);
}

void CameraSensorTableWidget::setCameraSensors(const QList<CameraSensor>& sensors)
{
    cameraSensorsList = sensors;
    updateTable();
}

void CameraSensorTableWidget::updateTable()
{
    sensorTable->setRowCount(cameraSensorsList.size());  // Anzahl der Zeilen entspricht der Anzahl der Kamera-Sensoren

    for (int row = 0; row < cameraSensorsList.size(); ++row)
    {
        const CameraSensor& sensor = cameraSensorsList[row];

        sensorTable->setItem(row, 0, new QTableWidgetItem(sensor.name()));
        sensorTable->setItem(row, 1, new QTableWidgetItem(QString::number(sensor.resolution())));
        sensorTable->setItem(row, 2, new QTableWidgetItem(QString::number(sensor.horizontalPixels())));
        sensorTable->setItem(row, 3, new QTableWidgetItem(QString::number(sensor.verticalPixels())));
        sensorTable->setItem(row, 4, new QTableWidgetItem(QString::number(sensor.pixelSize())));
        sensorTable->setItem(row, 5, new QTableWidgetItem(QString::number(sensor.sensorArea())));
        sensorTable->setItem(row, 6, new QTableWidgetItem(QString::number(sensor.width())));
        sensorTable->setItem(row, 7, new QTableWidgetItem(QString::number(sensor.height())));
        sensorTable->setItem(row, 8, new QTableWidgetItem(QString::number(sensor.diagonal())));
        sensorTable->setItem(row, 9, new QTableWidgetItem(QString::number(sensor.cropFactor())));
        sensorTable->setItem(row, 10, new QTableWidgetItem(QString::number(sensor.releaseYear())));
        sensorTable->setItem(row, 11, new QTableWidgetItem(QString::number(sensor.sensitivity())));
        sensorTable->setItem(row, 12, new QTableWidgetItem(sensor.format()));
        sensorTable->setItem(row, 13, new QTableWidgetItem(sensor.other()));
    }
}

QList<CameraSensor> CameraSensorTableWidget::getCameraSensors() const
{
    return cameraSensorsList;  // Gibt die interne Liste der Kamera-Sensoren zurück
}

void CameraSensorTableWidget::addCameraSensor(const CameraSensor& sensor)
{
    cameraSensorsList.append(sensor);  // Füge den neuen Kamera-Sensor zur Liste hinzu
    updateTable();  // Aktualisiere die Tabelle
}
