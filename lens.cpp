#include "lens.h"
#include <QJsonObject>

// Zoomobjektiv-Konstruktor
Lens::Lens(QString id, int focalLengthMin, int focalLengthMax,
           double apertureMin, double apertureMax,
           StabilizationMethod stabi)
    : m_id(id),
    m_type("Zoomobjektiv"),
    m_focalLengthMin(focalLengthMin),
    m_focalLengthMax(focalLengthMax),
    m_apertureMin(apertureMin),
    m_apertureMax(apertureMax),
    m_stabi(stabi)
{
    if (focalLengthMin == focalLengthMax) {
        m_type = "Festbrennweite";
    }
}

// Festbrennweite-Konstruktor
Lens::Lens(QString id, int focalLength, double aperture,
           StabilizationMethod stabi)
    : m_id(id),
    m_type("Festbrennweite"),
    m_focalLengthMin(focalLength),
    m_focalLengthMax(focalLength),
    m_apertureMin(aperture),
    m_apertureMax(aperture),
    m_stabi(stabi)
{}


Lens Lens::fromJson(const QJsonObject& obj) {
    QString id = obj["id"].toString();
    double apertureMin = obj["apertureMin"].toDouble();
    int focalLengthMin = obj["focalLengthMin"].toInt();

    StabilizationMethod stabi = NoStabilization;
    if (obj.contains("stabilization")) {
        stabi = static_cast<StabilizationMethod>(obj["stabilization"].toInt());
    }
    if (obj.contains("focalLengthMax") && obj.contains("apertureMax")) {
        int focalLengthMax = obj["focalLengthMax"].toInt();
        double apertureMax = obj["apertureMax"].toDouble();
        return Lens(id, focalLengthMin, focalLengthMax, apertureMin, apertureMax, stabi);
    } else if (obj.contains("focalLengthMax")) {
        int focalLengthMax = obj["focalLengthMax"].toInt();
        return Lens(id, focalLengthMin, focalLengthMax, apertureMin, apertureMin, stabi);
    } else if (obj.contains("apertureMax")) {
        double apertureMax = obj["apertureMax"].toDouble();
        return Lens(id, focalLengthMin, focalLengthMin, apertureMin, apertureMax, stabi);
    } else {
        return Lens(id, focalLengthMin, apertureMin, stabi);
    }
}

QJsonObject Lens::toJson() const {
    QJsonObject obj;
    obj["id"] = m_id;
    obj["type"] = m_type;
    obj["focalLengthMin"] = m_focalLengthMin;
    obj["focalLengthMax"] = m_focalLengthMax;
    obj["apertureMin"] = m_apertureMin;
    obj["apertureMax"] = m_apertureMax;
    obj["stabilization"] = static_cast<int>(m_stabi); // Enum als int speichern
    return obj;
}

Lens::Lens(const Lens& other)
    : m_id(other.m_id),
    m_type(other.m_type),
    m_focalLengthMin(other.m_focalLengthMin),
    m_focalLengthMax(other.m_focalLengthMax),
    m_apertureMin(other.m_apertureMin),
    m_apertureMax(other.m_apertureMax),
    m_stabi(other.m_stabi) // <- wichtig
{}

Lens& Lens::operator=(const Lens& other) {
    if (this != &other) {
        m_id = other.m_id;
        m_type = other.m_type;
        m_focalLengthMin = other.m_focalLengthMin;
        m_focalLengthMax = other.m_focalLengthMax;
        m_apertureMin = other.m_apertureMin;
        m_apertureMax = other.m_apertureMax;
        m_stabi = other.m_stabi; // <- hinzufügen
    }
    return *this;
}
