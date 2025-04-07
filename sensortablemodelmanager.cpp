// sensor_table_model_manager.cpp
#include "sensortablemodelmanager.h"
#include <QJsonArray>
#include <QFile>

SensorTableModelManager::SensorTableModelManager(QTableView* tableView)
    : view(tableView)
{
    model = new QStandardItemModel();
    setupHeaders();
    view->setModel(model);
}

void SensorTableModelManager::setupHeaders()
{
    QStringList headers = {
        "Name", "Auflösung", "Pixelgröße (µm)", "Breite (mm)", "Höhe (mm)",
        "Diagonale (mm)", "Fläche (mm²)", "Crop-Faktor", "Format", "Jahr"
    };
    model->setHorizontalHeaderLabels(headers);
}

void SensorTableModelManager::addSensor(const CameraSensor& sensor)
{
    QList<QStandardItem*> row;
    row << new QStandardItem(sensor.name());
    row << new QStandardItem(QString("%1 × %2").arg(sensor.horizontalPixels()).arg(sensor.verticalPixels()));
    row << new QStandardItem(QString::number(sensor.pixelSize()));
    row << new QStandardItem(QString::number(sensor.width(), 'f', 2));
    row << new QStandardItem(QString::number(sensor.height(), 'f', 2));
    row << new QStandardItem(QString::number(sensor.diagonal(), 'f', 2));
    row << new QStandardItem(QString::number(sensor.sensorArea(), 'f', 2));
    row << new QStandardItem(QString::number(sensor.cropFactor(), 'f', 2));
    row << new QStandardItem(sensor.format());
    row << new QStandardItem(QString::number(sensor.releaseYear()));

    model->appendRow(row);
}

