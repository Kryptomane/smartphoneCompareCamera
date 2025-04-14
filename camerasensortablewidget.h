#ifndef CAMERASENSORTABLEWIDGET_H
#define CAMERASENSORTABLEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QList>
#include "camerasensor.h"  // Assuming that CameraSensor class is already defined elsewhere

class CameraSensorTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CameraSensorTableWidget(QWidget *parent = nullptr);
    void setCameraSensors(const QList<CameraSensor>& sensors);  // Setzen der Liste von Kamera-Sensoren
    void updateTable();  // Die Tabelle aktualisieren
    QList<CameraSensor> getCameraSensors() const;  // Getter für die Liste der Kamera-Sensoren
    CameraSensor getCameraByName(const QString& cameraName) const;
    void addCameraSensor(const CameraSensor& sensor);  // Kamera-Sensor hinzufügen

private:
    QTableWidget* sensorTable;  // Tabelle zur Anzeige der Kamera-Sensoren
    QList<CameraSensor> cameraSensorsList;   // Liste aller Kamera-Sensoren
};

#endif // CAMERASENSORTABLEWIDGET_H
