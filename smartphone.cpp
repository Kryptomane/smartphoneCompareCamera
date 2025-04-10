#include "smartphone.h"

Smartphone::Smartphone()
    : m_releaseYear(0), m_displaySize(0.0), m_batteryCapacity(0) {}

void Smartphone::addMainCam(const SensorLensPair& pair) {
    m_mainCams.append(pair);
}

void Smartphone::addSelfieCam(const SensorLensPair& pair) {
    m_selfieCams.append(pair);
}

QList<SensorLensPair> Smartphone::getMainCams() const {
    return m_mainCams;
}

QList<SensorLensPair> Smartphone::getSelfieCams() const {
    return m_selfieCams;
}

QJsonObject Smartphone::toJson() const {
    QJsonObject jsonObject;

    // Hinzufügen der zusätzlichen Felder
    jsonObject["name"] = m_name;
    jsonObject["manufacture"] = m_manufacture;
    jsonObject["releaseYear"] = m_releaseYear;
    jsonObject["displaySize"] = m_displaySize;
    jsonObject["batteryCapacity"] = m_batteryCapacity;

    // Serialisierung der m_mainCams (QList<SensorLensPair>)
    QJsonArray mainCamsArray;
    for (const SensorLensPair& pair : m_mainCams) {
        mainCamsArray.append(pair.toJson());
    }
    jsonObject["mainCams"] = mainCamsArray;

    // Serialisierung von m_selfieCams (QPair<SensorLensPair>)
    QJsonArray selfieCamsArray;
    for (const SensorLensPair& pair : m_selfieCams) {
        selfieCamsArray.append(pair.toJson());
    }
    jsonObject["selfieCams"] = selfieCamsArray;
    return jsonObject;
}


Smartphone Smartphone::fromJson(const QJsonObject& jsonObject) {
    Smartphone phone;
    phone.m_name = jsonObject["name"].toString();
    phone.m_manufacture = jsonObject["manufacture"].toString();
    phone.m_releaseYear = jsonObject["releaseYear"].toInt();
    phone.m_displaySize = jsonObject["displaySize"].toDouble();
    phone.m_batteryCapacity = jsonObject["batteryCapacity"].toInt();

    // Deserialisierung der m_mainCams (QList<SensorLensPair>)
    QJsonArray mainCamsArray = jsonObject["mainCams"].toArray();
    for (const QJsonValue& value : mainCamsArray) {
        phone.m_mainCams.append(SensorLensPair::fromJson(value.toObject()));
    }

    // Deserialisierung von m_selfieCams (QList<SensorLensPair>)
    QJsonArray selfieCamsArray = jsonObject["selfieCams"].toArray();
    for (const QJsonValue& value : selfieCamsArray) {
        phone.m_selfieCams.append(SensorLensPair::fromJson(value.toObject()));
    }
    return phone;
}
