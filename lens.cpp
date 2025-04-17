#include "lens.h"
#include <QJsonObject>

// Zoomobjektiv-Konstruktor
Lens::Lens(QString id, int focalLengthMin, int focalLengthMax,
           double apertureMin, double apertureMax)
    : m_id(id),
    m_type("Zoomobjektiv"),
    m_focalLengthMin(focalLengthMin),
    m_focalLengthMax(focalLengthMax),
    m_apertureMin(apertureMin),
    m_apertureMax(apertureMax)
{
}

// Festbrennweite-Konstruktor
Lens::Lens(QString id, int focalLength, double aperture)
    : m_id(id),
    m_type("Festbrennweite"),
    m_focalLengthMin(focalLength),
    m_focalLengthMax(focalLength),
    m_apertureMin(aperture),
    m_apertureMax(aperture)
{
}

// Copy Constructor
Lens::Lens(const Lens& other)
    : m_id(other.m_id),
    m_type(other.m_type),
    m_focalLengthMin(other.m_focalLengthMin),
    m_focalLengthMax(other.m_focalLengthMax),
    m_apertureMin(other.m_apertureMin),
    m_apertureMax(other.m_apertureMax) {}

// Zuweisungsoperator
Lens& Lens::operator=(const Lens& other) {
    if (this != &other) {  // Vermeide Selbstzuweisung
        m_id = other.m_id;
        m_type = other.m_type;
        m_focalLengthMin = other.m_focalLengthMin;
        m_focalLengthMax = other.m_focalLengthMax;
        m_apertureMin = other.m_apertureMin;
        m_apertureMax = other.m_apertureMax;
    }
    return *this;
}

QJsonObject Lens::toJson() const {
    QJsonObject obj;
    obj["id"] = m_id;
    obj["type"] = m_type;
    obj["focalLengthMin"] = m_focalLengthMin;
    obj["focalLengthMax"] = m_focalLengthMax;
    obj["apertureMin"] = m_apertureMin;
    obj["apertureMax"] = m_apertureMax;
    return obj;
}

Lens Lens::fromJson(const QJsonObject& obj) {
    QString id = obj["id"].toString();  // ID der Linse
    double apertureMin = obj["apertureMin"].toDouble();  // Minimale Blende
    int focalLengthMin = obj["focalLengthMin"].toInt();  // Minimale Brennweite

    // Überprüfen, ob focalLengthMax oder apertureMax vorhanden sind
    if (obj.contains("focalLengthMax") && obj.contains("apertureMax")) {
        int focalLengthMax = obj["focalLengthMax"].toInt();  // Maximale Brennweite
        double apertureMax = obj["apertureMax"].toDouble();  // Maximale Blende
        return Lens(id, focalLengthMin, focalLengthMax, apertureMin, apertureMax); // Konstruktor für variable Brennweite und Blende
    }
    // Wenn nur focalLengthMax fehlt, kopiere den Wert von focalLengthMin
    else if (obj.contains("focalLengthMax")) {
        int focalLengthMax = obj["focalLengthMax"].toInt();
        return Lens(id, focalLengthMin, focalLengthMax, apertureMin, apertureMin); // Kopiere apertureMin
    }
    // Wenn nur apertureMax fehlt, kopiere den Wert von apertureMin
    else if (obj.contains("apertureMax")) {
        double apertureMax = obj["apertureMax"].toDouble();
        return Lens(id, focalLengthMin, focalLengthMin, apertureMin, apertureMax); // Kopiere focalLengthMin
    }
    // Wenn weder focalLengthMax noch apertureMax vorhanden sind, verwende den Festbrennweiten-Konstruktor
    else {
        return Lens(id, focalLengthMin, apertureMin);  // Konstruktor für Festbrennweite
    }
}
