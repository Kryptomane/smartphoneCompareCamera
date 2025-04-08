#ifndef SMARTPHONE_H
#define SMARTPHONE_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include "lens.h"
#include "camerasensor.h"

class Smartphone {
public:
    Smartphone();

    void setName(const QString& name);
    void setManufacturer(const QString& manufacturer);
    void setReleaseYear(int year);
    void setDisplaySize(double inches);
    void setBatteryCapacity(int capacity);

    QString getName() const;
    QString getManufacturer() const;
    int getReleaseYear() const;
    double getDisplaySize() const;
    int getBatteryCapacity() const;

    void addSensorLensPair(const CameraSensor& sensor, const Lens& lens);
    QList<QPair<CameraSensor, Lens>> getSensorLensPairs() const;

    void addFieldOfView(double fov);
    void addAngularResolution(double pixelsPerDegree);
    QList<double> getFieldOfViews() const;
    QList<double> getAngularResolutions() const;

    QJsonObject toJson() const;
    static Smartphone fromJson(const QJsonObject& obj);

private:
    QString name;
    QString manufacturer;
    int releaseYear;
    double displaySize;
    int batteryCapacity;

    QList<QPair<CameraSensor, Lens>> sensorLensPairs;
    QList<double> fieldOfViews;
    QList<double> angularResolutions;
};

#endif // SMARTPHONE_H
