#ifndef LENS_H
#define LENS_H

#include <QString>
#include <QJsonObject>

enum StabilizationMethod {
    NoStabilization = 0,
    OIS = 1,
    Gimbal = 2
};

Q_DECLARE_METATYPE(StabilizationMethod)

class Lens {
public:
    Lens(const Lens& other); // Copy Constructor
    Lens& operator=(const Lens& other); // Zuweisungsoperator

    // Konstruktor für Zoomobjektiv
    Lens(QString id, int focalLengthMin, int focalLengthMax,
         double apertureMin, double apertureMax, StabilizationMethod stabi);

    // Konstruktor für Festbrennweite
    Lens(QString id, int focalLength, double aperture, StabilizationMethod stabi);

    // Standard Konstruktor mit Initialisierung
    Lens(QString id = "", const QString& type = "", int focalLengthMin = 0, int focalLengthMax = 0, double maxAperture = 0.0, double minAperture = 0.0, StabilizationMethod stabi = NoStabilization)
        : m_id(id), m_type(type), m_focalLengthMin(focalLengthMin), m_focalLengthMax(focalLengthMax), m_apertureMax(maxAperture), m_apertureMin(minAperture), m_stabi(stabi) {}

    // Getter-Methoden
    QString id() const { return m_id; }
    QString type() const { return m_type; }
    int focalLengthMin() const { return m_focalLengthMin; }
    int focalLengthMax() const { return m_focalLengthMax; }
    double apertureMin() const { return m_apertureMin; }
    double apertureMax() const { return m_apertureMax; }
    StabilizationMethod stabi() const { return m_stabi; }

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
    StabilizationMethod m_stabi;
};

#endif // LENS_H
