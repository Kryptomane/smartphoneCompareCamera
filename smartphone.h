#ifndef SMARTPHONE_H
#define SMARTPHONE_H

#include <QString>
#include <QList>
#include <QPair>
#include <QJsonObject>
#include <QJsonArray>

#include "camerasensor.h"
#include "lens.h"

class Smartphone {
public:
    Smartphone() = default;

    Smartphone(const QString& name, const QString& manufacturer, int releaseYear,
               double displayDiagonal, int batteryCapacity);

    QString name() const;
    QString manufacturer() const;
    int releaseYear() const;
    double displayDiagonal() const;
    int batteryCapacity() const;

    const QList<QPair<CameraSensor, Lens>>& sensorLensPairs() const;
    void addSensorLensPair(const CameraSensor& sensor, const Lens& lens);

    QJsonObject toJson() const;
    static Smartphone fromJson(const QJsonObject& obj);

private:
    QString m_name;
    QString m_manufacturer;
    int m_releaseYear;
    double m_displayDiagonal;
    int m_batteryCapacity;
    QList<QPair<CameraSensor, Lens>> m_sensorLensPairs;
};

#endif // SMARTPHONE_H
