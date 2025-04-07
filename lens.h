#ifndef LENS_H
#define LENS_H

#include <QString>
#include <QJsonObject>

class Lens {
public:
    Lens(const Lens& other); // Copy Constructor
    Lens& operator=(const Lens& other); // Zuweisungsoperator

    // Konstruktor für Zoomobjektiv
    Lens(QString id, int focalLengthMin, int focalLengthMax,
         double apertureMin, double apertureMax);

    // Konstruktor für Festbrennweite
    Lens(QString id, int focalLength, double aperture);

    // Standard Konstruktor mit Initialisierung
    Lens(QString id = "", const QString& type = "", int focalLengthMin = 0, int focalLengthMax = 0, double maxAperture = 0.0, double minAperture = 0.0)
        : m_id(id), m_type(type), m_focalLengthMin(focalLengthMin), m_focalLengthMax(focalLengthMax), m_apertureMax(maxAperture), m_apertureMin(minAperture) {}

    // Getter-Methoden
    QString id() const { return m_id; }
    QString type() const { return m_type; }
    int focalLengthMin() const { return m_focalLengthMin; }
    int focalLengthMax() const { return m_focalLengthMax; }
    double apertureMin() const { return m_apertureMin; }
    double apertureMax() const { return m_apertureMax; }

    // Methoden für JSON Konvertierung
    QJsonObject toJson() const;
    static Lens fromJson(const QJsonObject& obj);

private:
    QString m_id;
    QString m_type;
    int m_focalLengthMin;
    int m_focalLengthMax;
    double m_apertureMin;
    double m_apertureMax;
};

#endif // LENS_H
