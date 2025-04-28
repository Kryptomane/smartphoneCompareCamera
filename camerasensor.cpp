#include "CameraSensor.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <cmath>
#include <QString>
#include <QMessageBox>
#include <sstream>
#include <iomanip>

CameraSensor::CameraSensor(const QString& name, const QString& inchSize, int resolution, const QString& format) {
    *this = createCameraSensorFromInchSize(name, inchSize, resolution, format);
}

CameraSensor::CameraSensor(const QString& name, int horizontalPixels, double pixelSize, const QString& format) {
    *this = createCameraSensorFromPixels(name, horizontalPixels, pixelSize, format);
}

CameraSensor CameraSensor::createCameraSensorFromInchSize(const QString& name, const QString& inchSize, int resolution, const QString& format) {
    // Parse das Inch-Format, z.B. "1/2,76"
    QString cleaned = inchSize;
    cleaned.replace(",", ".");  // Komma durch Punkt ersetzen
    QStringList parts = cleaned.split('/');

    if (parts.size() != 2) {
        qWarning() << "Ungültiges Format für inchSize:" << inchSize;
        return CameraSensor(); // Rückgabe eines leeren Objekts bei Fehler
    }

    bool ok1, ok2;
    double num = parts[0].toDouble(&ok1);
    double den = parts[1].toDouble(&ok2);

    if (!ok1 || !ok2 || den == 0.0) {
        qWarning() << "Fehler beim Parsen von inchSize:" << inchSize;
        return CameraSensor();
    }

    double realFraction = num / den;
    double realDiameter = realFraction * 25.4;

    double opticalDiameter = (realFraction >= 0.5)
                                 ? (realDiameter / 1.5875)
                                 : (realDiameter / 1.4111);

    // Nun muss noch Sensorfläche, Breite, Höhe, Pixelgröße etc. berechnet werden
    // Wir nehmen das Format 4:3 an, d.h. Seitenverhältnis ist 4:3
    double aspectX = 4.0;
    double aspectY = 3.0;
    if (format == "16:9") {
        aspectX = 16.0;
        aspectY = 9.0;
    }

    double diag = opticalDiameter;
    double ratio = aspectX / aspectY;

    // Rechne Breite und Höhe aus der Diagonale
    double height = diag / std::sqrt(1 + ratio * ratio);
    double width = ratio * height;

    double area = width * height;

    int verticalPixels = static_cast<int>(std::sqrt(resolution * aspectY / aspectX));
    int horizontalPixels = static_cast<int>(std::round((double)verticalPixels * aspectX / aspectY));

    double pixelSize = width * 1000.0 / horizontalPixels; // µm
    int bitDepth = 10;

    // Crop Factor relativ zu Kleinbild (43,3 mm Diagonale)
    double cropFactor = 43.3 / diag;

    return CameraSensor(
        name,
        "model",
        resolution,
        horizontalPixels,
        verticalPixels,
        pixelSize,
        area,
        width,
        height,
        diag,
        cropFactor,
        2010,             // releaseYear
        1.0,           // sensitivity
        format,
        inchSize,
        "RGGB",        // Standard pattern
        bitDepth,
        "filledByInchSize"
        );
}

std::string CameraSensor::approximateSensorFormat(double diameter_mm) {
    double correctionFactor;

    // Entscheidung, welchen Korrekturfaktor wir nehmen
    if (diameter_mm >= 8.0) { // größere Sensoren ab ca. 1/2"
        correctionFactor = 1.5875;
    } else {
        correctionFactor = 1.4111;
    }

    // Umrechnung: tatsächlicher "nominaler" Inch-Durchmesser
    double inch_nominal = diameter_mm * correctionFactor / 25.4; // 1 inch = 25,4mm

    // Jetzt die Umkehrung: Inch-Angabe als Bruch 1/x
    double fraction = 1.0 / inch_nominal;

    // Auf sinnvolle Werte runden (z.B. auf ganze 0.1 Schritte)
    double rounded_fraction = std::round(fraction * 10.0) / 10.0;

    // Formatieren als String
    std::ostringstream oss;
    oss << "1/" << std::fixed << std::setprecision(2) << rounded_fraction << "\"";

    return oss.str();
}

CameraSensor CameraSensor::createCameraSensorFromPixels(const QString& name, int horizontalPixels, double pixelSize, const QString& format){
    // Seitenverhältnis bestimmen
    double aspectX = 4.0;
    double aspectY = 3.0;

    if (format == "16:9") {
        aspectX = 16.0;
        aspectY = 9.0;
    } else if (format == "3:2") {
        aspectX = 3.0;
        aspectY = 2.0;
    } else if (format == "1:1") {
        aspectX = 1.0;
        aspectY = 1.0;
    }

    // Berechne vertikale Pixelanzahl aus horizontaler und Format
    int verticalPixels = static_cast<int>(std::round(horizontalPixels * aspectY / aspectX));

    // Sensorbreite und -höhe (in mm)
    double width = (horizontalPixels * pixelSize) / 1000.0; // µm → mm
    double height = (verticalPixels * pixelSize) / 1000.0;

    // Diagonale des Sensors (in mm)
    double diameter = std::sqrt(width * width + height * height);

    // Sensorfläche (in mm²)
    double area = width * height;

    // Auflösung insgesamt (Pixelanzahl)
    int resolution = horizontalPixels * verticalPixels;

    QString inchSizeStr = QString::fromStdString(approximateSensorFormat(diameter));

    // Crop Factor relativ zu 43.3 mm Vollformat
    double cropFactor = 43.3 / diameter;

    return CameraSensor(
        name,
        "model",
        resolution,
        horizontalPixels,
        verticalPixels,
        pixelSize,
        area,
        width,
        height,
        diameter,
        cropFactor,
        2010,              // releaseYear
        1.0,            // sensitivity
        format,
        inchSizeStr,
        "RGGB",         // Standard Bayer Pattern
        10,             // Bit-Tiefe
        "filledByPixelSize"              // other
        );
}

bool CameraSensor::compareValueDeviation(double expected, double calculated, double deviation){
    if (qAbs(expected - calculated) / expected <= deviation) {
        return true;
    }
    return false;
}

CameraSensor CameraSensor::takeOver(CameraSensor master, CameraSensor slave){
    if (master.manufacture() == "") {
        master.setManufacture(slave.manufacture());
    }
    if (master.verticalPixels() == 0 || master.verticalPixels() < 1000 || master.verticalPixels() > 15000) { //invalid
        master.setVerticalPixels(slave.verticalPixels());
    }
    if (master.horizontalPixels() == 0 || master.horizontalPixels() < 1000 || master.horizontalPixels() > 20000) { //invalid
        master.setHorizontalPixels(slave.horizontalPixels());
    }
    if ((master.horizontalPixels() != 0) && (master.verticalPixels() != 0) && master.resolution() != 0){
        if (compareValueDeviation(master.horizontalPixels()*master.verticalPixels(), master.resolution())){
            master.setResolution(master.horizontalPixels()*master.verticalPixels());
        }
    } else {
        master.setResolution(slave.resolution());
    }
    if (master.pixelSize() == 0 || master.pixelSize() < 0.4 || master.pixelSize() > 4) {
        master.setPixelSize(slave.pixelSize());
    }
    if (master.sensorArea() == 0 || master.sensorArea() < 1 || master.sensorArea() > 445) {
        master.setSensorArea(slave.sensorArea());
    }
    if (master.diagonal() == 0 || master.diagonal() < 0 || master.diagonal() > 100) {
        master.setDiagonal(slave.diagonal());
    }
    if (master.width() == 0 || master.width() < 0 || master.width()>100) {
        master.setWidth(slave.width());
    }
    if (master.height() == 0 || master.height() < 0 || master.height() > 100) {
        master.setHeight(slave.height());
    }
    if (master.cropFactor() == 0 || master.cropFactor() < 1 || master.cropFactor() > 10) {
        master.setCropFactor(slave.cropFactor());
    }
    if (master.releaseYear() <= 2011 || slave.releaseYear() >= 2030){
        master.setReleaseYear(slave.releaseYear());
    }
    if (master.sensitivity() < 0.1 || slave.sensitivity() >= 4){
        master.setSensitivity(slave.sensitivity());
    }
    if (master.other() == "filledByPixelSize" || master.other() == "filledByInchSize"){
        if (slave.other() != ""){
            master.setOther(slave.other());
        }
    }
    return master;
}

CameraSensor CameraSensor::makePlausibelCameraSensor(CameraSensor original) {
    if ((original.resolution() > 0) && (original.resolution() < 1000000)){
        original.setResolution(original.resolution()*1000000);
    }
    CameraSensor temp, temp2;
    if ((original.inchSize() != " ") && (original.resolution() > 0)){
        temp = createCameraSensorFromInchSize(original.name(), original.inchSize(), original.resolution(), original.format());
    }
    if ((original.horizontalPixels() > 0) && (original.pixelSize() > 0)){
        temp2 = createCameraSensorFromPixels(original.name(), original.horizontalPixels(), original.pixelSize(), original.format());
    }
    if ((temp.other() == "filledByInchSize") && (temp2.other() == "filledByPixelSize")) {
        if (!compareValueDeviation(temp.sensorArea(), temp2.sensorArea())){
            QMessageBox::warning(nullptr, "Fehler", "Die berechneten Werte stimmen nicht überein. Die Abweichung beträgt mehr als 5%.");
            //überlegen was hier valid ist
            return original;
        } else {
            //valid
            //überschreiben von original was fehlt
            return takeOver(original,temp);
        }
    }
    if (temp.other() == "filledByInchSize") {
        return takeOver(original,temp);//überschreiben von temp vom original
    }
    if (temp2.other() == "filledByPixelSize") {
        return takeOver(original,temp2);
        //überschreiben von temp2 vom original
    }
    return original;
}

QJsonObject CameraSensor::toJson() const {
    QJsonObject obj;
    obj["name"] = m_name;
    obj["manufacture"] = m_manufacture;
    obj["resolution"] = m_resolution;
    obj["horizontalPixels"] = m_horizontalPixels;
    obj["verticalPixels"] = m_verticalPixels;
    obj["pixelSize"] = m_pixelSize;
    obj["sensorArea"] = m_sensorArea;
    obj["width"] = m_width;
    obj["height"] = m_height;
    obj["diagonal"] = m_diagonal;
    obj["cropFactor"] = m_cropFactor;
    obj["releaseYear"] = m_releaseYear;
    obj["sensitivity"] = m_sensitivity;
    obj["format"] = m_format;
    obj["inchSize"] = m_inchSize;
    obj["pattern"] = m_pattern;
    obj["bitDepth"] = m_bitDepth;
    obj["other"] = m_other;
    return obj;
}

CameraSensor CameraSensor::fromJson(const QJsonObject& obj) {
    return CameraSensor(
        obj["name"].toString(),
        obj["manufacture"].toString(),
        obj["resolution"].toInt(),
        obj["horizontalPixels"].toInt(),
        obj["verticalPixels"].toInt(),
        obj["pixelSize"].toDouble(),
        obj["sensorArea"].toDouble(),
        obj["width"].toDouble(),
        obj["height"].toDouble(),
        obj["diagonal"].toDouble(),
        obj["cropFactor"].toDouble(),
        obj["releaseYear"].toInt(),
        obj["sensitivity"].toDouble(),
        obj["format"].toString(),
        obj["inchSize"].toString(),
        obj["pattern"].toString(),
        obj["bitDepth"].toInt(),
        obj["other"].toString()
        );
}
