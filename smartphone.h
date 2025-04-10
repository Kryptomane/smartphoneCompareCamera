#ifndef SMARTPHONE_H
#define SMARTPHONE_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include "lens.h"
#include "camerasensor.h"

struct SensorLensPair {
    QString sensorName;    // Name des CameraSensors
    QString lensId;        // ID der Lens
    double fieldOfView;    // Öffnungswinkel in Grad

    // Konstruktor zur Initialisierung
    SensorLensPair(const QString& sensorName, const QString& lensId, double fov)
        : sensorName(sensorName), lensId(lensId), fieldOfView(fov) {}
    QJsonObject toJson() const {
        QJsonObject jsonObject;

        // Hinzufügen der Felder zum JSON-Objekt
        jsonObject["sensorName"] = sensorName;
        jsonObject["lensId"] = lensId;
        jsonObject["fieldOfView"] = fieldOfView;

        return jsonObject;
    }
    static SensorLensPair fromJson(const QJsonObject& jsonObject) {
        // Deserialisierung der Felder aus dem JSON-Objekt
        QString sensorName = jsonObject["sensorName"].toString();
        QString lensId = jsonObject["lensId"].toString();
        double fieldOfView = jsonObject["fieldOfView"].toDouble();

        // Rückgabe des erzeugten SensorLensPair-Objekts
        return SensorLensPair(sensorName, lensId, fieldOfView);
    }
};

class Smartphone {
public:
    Smartphone();

    void setName(const QString& name) {m_name = name;}
    void setManufacturer(const QString& manufacture) {m_manufacture = manufacture;}
    void setReleaseYear(int year) {m_releaseYear = year;}
    void setDisplaySize(double inches) {m_displaySize = inches;}
    void setBatteryCapacity(int capacity) {m_batteryCapacity = capacity;}

    QString name() const {return m_name;}
    QString manufacturer() const {return m_manufacture; }
    int releaseYear() const {return m_releaseYear;}
    double displaySize() const {return m_displaySize;}
    int batteryCapacity() const {return m_batteryCapacity;}

    void addMainCam(const SensorLensPair& pair);
    void addSelfieCam(const SensorLensPair& pair);
    QList<SensorLensPair> getMainCams() const;
    QList<SensorLensPair> getSelfieCams() const;

    QJsonObject toJson() const;
    static Smartphone fromJson(const QJsonObject& obj);

private:
    QString m_name;
    QString m_manufacture;
    int m_releaseYear;
    double m_displaySize;
    int m_batteryCapacity;
    QList<SensorLensPair> m_mainCams;
    QList<SensorLensPair> m_selfieCams;
};

#endif // SMARTPHONE_H
