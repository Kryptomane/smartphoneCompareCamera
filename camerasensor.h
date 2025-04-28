#ifndef CAMERASENSOR_H
#define CAMERASENSOR_H

#include <QString>
#include <QJsonObject>

class CameraSensor
{
public:
    CameraSensor() = default;

    CameraSensor(const QString& name, const QString& manufacture, int resolution, int horizontalPixels, int verticalPixels, double pixelSize, double area, double width, double height,
                              double diagonal, double cropFactor, int releaseYear, double sensitivity, const QString& format, const QString& inchSize, const QString& pattern, int bitDepth, const QString& other)
        : m_name(name),
        m_manufacture(manufacture),
        m_resolution(resolution),
        m_horizontalPixels(horizontalPixels),
        m_verticalPixels(verticalPixels),
        m_pixelSize(pixelSize),
        m_sensorArea(area),
        m_width(width),
        m_height(height),
        m_diagonal(diagonal),
        m_cropFactor(cropFactor),
        m_releaseYear(releaseYear),
        m_sensitivity(sensitivity),
        m_format(format),
        m_inchSize(inchSize),
        m_pattern(pattern),
        m_bitDepth(bitDepth),
        m_other(other)
    {}

    CameraSensor(const QString& name, const QString& inchSize, int resolution, const QString& format);
    CameraSensor(const QString& name, int horizontalPixels, double pixelSize, const QString& format);

    // Getter
    QString manufacture() const {return m_manufacture; }
    QString name() const { return m_name; }
    int resolution() const { return m_resolution; }
    int horizontalPixels() const { return m_horizontalPixels; }
    int verticalPixels() const { return m_verticalPixels; }
    double pixelSize() const { return m_pixelSize; }
    double sensorArea() const { return m_sensorArea; }
    double width() const { return m_width; }
    double height() const { return m_height; }
    double diagonal() const { return m_diagonal; }
    double cropFactor() const { return m_cropFactor; }
    int releaseYear() const { return m_releaseYear; }
    double sensitivity() const { return m_sensitivity; }
    QString format() const { return m_format; }
    QString other() const { return m_other; }
    QString inchSize() const { return m_inchSize; }
    int bitDepth() const { return m_bitDepth; }
    QString pattern() const { return m_pattern; }

    // Setter
    void setName(const QString& name) { m_name = name; }
    void setResolution(int resolution) { m_resolution = resolution; }
    void setHorizontalPixels(int pixels) { m_horizontalPixels = pixels; }
    void setVerticalPixels(int pixels) { m_verticalPixels = pixels; }
    void setPixelSize(double size) { m_pixelSize = size; }
    void setSensorArea(double area) { m_sensorArea = area; }
    void setWidth(double width) { m_width = width; }
    void setHeight(double height) { m_height = height; }
    void setDiagonal(double diagonal) { m_diagonal = diagonal; }
    void setCropFactor(double factor) { m_cropFactor = factor; }
    void setReleaseYear(int year) { m_releaseYear = year; }
    void setSensitivity(double value) { m_sensitivity = value; }
    void setFormat(const QString& format) { m_format = format; }
    void setOther(const QString& other) { m_other = other; }
    void setInchSize(const QString& size) { m_inchSize = size; }
    void setBitDepth(int depth) { m_bitDepth = depth; }
    void setPattern(const QString& pattern) { m_pattern = pattern; }
    void setManufacture(const QString& manu) { m_manufacture = manu; }

    // (De-)Serialisierung (Deklaration – du kannst sie bei Bedarf noch inline definieren)
    QJsonObject toJson() const;
    static CameraSensor fromJson(const QJsonObject& obj);
    static std::string approximateSensorFormat(double diameter_mm);
    static CameraSensor createCameraSensorFromInchSize(const QString& name, const QString& inchSize, int resolution, const QString& format = "4:3");
    static CameraSensor createCameraSensorFromPixels(const QString& name, int horizontalPixels, double pixelSize, const QString& format = "4:3");
    static CameraSensor makePlausibelCameraSensor(CameraSensor original);
    static bool compareValueDeviation(double expected, double calculated, double deviation = 0.05);
    static CameraSensor takeOver(CameraSensor master, CameraSensor slave);

private:
    QString m_manufacture;
    QString m_name;
    int m_resolution;
    int m_horizontalPixels;
    int m_verticalPixels;
    double m_pixelSize;
    double m_sensorArea;
    double m_width;
    double m_height;
    double m_diagonal;
    double m_cropFactor;
    int m_releaseYear;
    double m_sensitivity;
    QString m_format;
    QString m_other;
    QString m_inchSize;
    int m_bitDepth;
    QString m_pattern;
};

#endif // CAMERASENSOR_H
