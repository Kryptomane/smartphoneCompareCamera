#include "smartphone.h"

Smartphone::Smartphone(const QString& name, const QString& manufacturer,
                       int releaseYear, double displayDiagonal, int batteryCapacity)
    : m_name(name),
    m_manufacturer(manufacturer),
    m_releaseYear(releaseYear),
    m_displayDiagonal(displayDiagonal),
    m_batteryCapacity(batteryCapacity)
{
}

void Smartphone::addSensorLensPair(const CameraSensor& sensor, const Lens& lens) {
    m_sensorLensPairs.append(qMakePair(sensor, lens));
}

QString Smartphone::name() const { return m_name; }
QString Smartphone::manufacturer() const { return m_manufacturer; }
int Smartphone::releaseYear() const { return m_releaseYear; }
double Smartphone::displayDiagonal() const { return m_displayDiagonal; }
int Smartphone::batteryCapacity() const { return m_batteryCapacity; }

const QList<QPair<CameraSensor, Lens>>& Smartphone::sensorLensPairs() const {
    return m_sensorLensPairs;
}

QJsonObject Smartphone::toJson() const {
    QJsonObject obj;
    obj["name"] = m_name;
    obj["manufacturer"] = m_manufacturer;
    obj["releaseYear"] = m_releaseYear;
    obj["displayDiagonal"] = m_displayDiagonal;
    obj["batteryCapacity"] = m_batteryCapacity;

    QJsonArray pairArray;
    for (const auto& pair : m_sensorLensPairs) {
        QJsonObject pairObj;
        pairObj["sensor"] = pair.first.toJson();
        pairObj["lens"] = pair.second.toJson();
        pairArray.append(pairObj);
    }
    obj["sensorLensPairs"] = pairArray;

    return obj;
}

Smartphone Smartphone::fromJson(const QJsonObject& obj) {
    Smartphone phone(
        obj["name"].toString(),
        obj["manufacturer"].toString(),
        obj["releaseYear"].toInt(),
        obj["displayDiagonal"].toDouble(),
        obj["batteryCapacity"].toInt()
        );

    QJsonArray pairArray = obj["sensorLensPairs"].toArray();
    for (const auto& val : pairArray) {
        QJsonObject pairObj = val.toObject();
        CameraSensor sensor = CameraSensor::fromJson(pairObj["sensor"].toObject());
        Lens lens = Lens::fromJson(pairObj["lens"].toObject());
        phone.addSensorLensPair(sensor, lens);
    }

    return phone;
}
