// sensor_table_model_manager.h
#ifndef SENSOR_TABLE_MODEL_MANAGER_H
#define SENSOR_TABLE_MODEL_MANAGER_H

#include "camerasensor.h"
#include <QStandardItemModel>
#include <QTableView>
#include <QList>

class SensorTableModelManager {
public:
    SensorTableModelManager(QTableView* tableView);

    void addSensor(const CameraSensor& sensor);
    void setupHeaders();
    bool saveToFile(const QString& filePath);

private:
    QStandardItemModel* model;
    QTableView* view;
};

#endif // SENSOR_TABLE_MODEL_MANAGER_H
