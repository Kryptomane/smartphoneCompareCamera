#include "smartphone.h"

Smartphone::Smartphone()
    : releaseYear(0), displaySize(0.0), batteryCapacity(0) {}

void Smartphone::setName(const QString& n) { name = n; }
void Smartphone::setManufacturer(const QString& m) { manufacturer = m; }
void Smartphone::setReleaseYear(int y) { releaseYear = y; }
void Smartphone::setDisplaySize(double d) { displaySize = d; }
void Smartphone::setBatteryCapacity(int c) { batteryCapacity = c; }

QString Smartphone::getName() const { return name; }
QString Smartphone::getManufacturer() const { return manufacturer; }
int Smartphone::getReleaseYear() const { return releaseYear; }
double Smartphone::getDisplaySize() const { return displaySize; }
int Smartphone::getBatteryCapacity() const { return batteryCapacity; }

void Smartphone::addSensorLensPair(const CameraSensor& sensor, const Lens& lens) {
    sensorLensPairs.append(qMakePair(sensor, lens));
}

QList<QPair<CameraSensor, Lens>> Smartphone::getSensorLensPairs() const {
    return sensorLensPairs;
}

void Smartphone::addFieldOfView(double fov) {
    fieldOfViews.append(fov);
}

void Smartphone::addAngularResolution(double pixelsPerDegree) {
    angularResolutions.append(pixelsPerDegree);
}

QList<double> Smartphone::getFieldOfViews() const {
    return fieldOfViews;
}

QList<double> Smartphone::getAngularResolutions() const {
    return angularResolutions;
}

QJsonObject Smartphone::toJson() const {
    QJsonObject obj;
    obj["name"] = name;
    obj["manufacturer"] = manufacturer;
    obj["releaseYear"] = releaseYear;
    obj["displaySize"] = displaySize;
    obj["batteryCapacity"] = batteryCapacity;

    QJsonArray pairArray;
    for (const auto& pair : sensorLensPairs) {
        QJsonObject item;
        item["sensor"] = pair.first.toJson();
        item["lens"] = pair.second.toJson();
        pairArray.append(item);
    }
    obj["sensorLensPairs"] = pairArray;

    QJsonArray fovArray;
    for (double fov : fieldOfViews)
        fovArray.append(fov);
    obj["fieldOfViews"] = fovArray;

    QJsonArray resolutionArray;
    for (double r : angularResolutions)
        resolutionArray.append(r);
    obj["angularResolutions"] = resolutionArray;

    return obj;
}

Smartphone Smartphone::fromJson(const QJsonObject& obj) {
    Smartphone phone;
    phone.name = obj["name"].toString();
    phone.manufacturer = obj["manufacturer"].toString();
    phone.releaseYear = obj["releaseYear"].toInt();
    phone.displaySize = obj["displaySize"].toDouble();
    phone.batteryCapacity = obj["batteryCapacity"].toInt();

    QJsonArray pairArray = obj["sensorLensPairs"].toArray();
    for (const auto& value : pairArray) {
        QJsonObject item = value.toObject();
        CameraSensor sensor = CameraSensor::fromJson(item["sensor"].toObject());
        Lens lens = Lens::fromJson(item["lens"].toObject());
        phone.addSensorLensPair(sensor, lens);
    }

    QJsonArray fovArray = obj["fieldOfViews"].toArray();
    for (const auto& value : fovArray)
        phone.fieldOfViews.append(value.toDouble());

    QJsonArray resolutionArray = obj["angularResolutions"].toArray();
    for (const auto& value : resolutionArray)
        phone.angularResolutions.append(value.toDouble());

    return phone;
}
