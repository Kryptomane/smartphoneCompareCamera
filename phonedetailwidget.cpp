#include "phonedetailwidget.h"
#include <QVBoxLayout>

PhoneInfoWidget::PhoneInfoWidget(QWidget* parent)
    : QWidget(parent), phoneSelector(new QComboBox(this)), phoneDetailsLabel(new QLabel(this)) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(phoneSelector);
    layout->addWidget(phoneDetailsLabel);

    phoneDetailsLabel->setWordWrap(true);

    // Vertikalen Spacer hinzufügen (sorgt dafür, dass das Label oben bleibt)
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    connect(phoneSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PhoneInfoWidget::onSmartphoneSelected);
}

void PhoneInfoWidget::setSmartphones(const QList<Smartphone>& phones) {
    smartphones = phones;
    phoneSelector->clear();
    for (const Smartphone& phone : smartphones) {
        phoneSelector->addItem(phone.manufacturer() + " " + phone.name());
    }
}

void PhoneInfoWidget::setSensorAndLensWidgets(SensorWidget* sensorWidget, LensWidget* lensWidget) {
    m_sensorWidget = sensorWidget;
    m_lensWidget = lensWidget;
}

QString PhoneInfoWidget::createModelString(const Smartphone& phone){
    QString detailText = QString(
                             "<b>Name:</b> %1<br>"
                             "<b>Hersteller:</b> %2<br>"
                             "<b>Release:</b> %3<br>"
                             "<b>Display:</b> %4 Zoll<br>"
                             "<b>Akku:</b> %5 mAh<br>"
                             "<b>Anzahl Hauptkameras:</b> %6<br>"
                             "<b>Anzahl Selfie-Kameras:</b> %7")
                             .arg(phone.name())
                             .arg(phone.manufacturer())
                             .arg(phone.releaseYear())
                             .arg(phone.displaySize(), 0, 'f', 1)
                             .arg(phone.batteryCapacity())
                             .arg(phone.getMainCams().size())
                             .arg(phone.getSelfieCams().size());
    return detailText;
}

QString PhoneInfoWidget::createSensorPairString(SensorLensPair pair){
    CameraSensor sensor = m_sensorWidget->getCameraByName(pair.sensorName);
    Lens lens = m_lensWidget->getLensById(pair.lensId);

    QString line1 = QString("%1 %2\"  %3\u00B5m  %4MP <br>")
                        .arg(sensor.manufacture() + " " + sensor.name())
                        .arg(sensor.inchSize())
                        .arg(sensor.pixelSize(),0,'f',2)
                        .arg(sensor.resolution() / 1000000);

    // Zweite Zeile: Brennweite, Blende, Bildwinkel
    QString line2 = QString("%1mm  f/%2  %3°  ")
                        .arg(lens.focalLengthMin())
                        .arg(lens.apertureMin())
                        .arg(pair.fieldOfView, 0, 'f', 2);

    // Stabilisierungstyp hinzufügen
    if (lens.stabi() == StabilizationMethod::NoStabilization) {
        line2.append("No Stabi");
    } else if (lens.stabi() == StabilizationMethod::OIS) {
        line2.append("OIS");
    } else if (lens.stabi() == StabilizationMethod::Gimbal) {
        line2.append("Gimbal");
    }
    return QString(line1 + line2 + "<br><br>");
}

void PhoneInfoWidget::onSmartphoneSelected(int index) {
    if (index < 0 || index >= smartphones.size())
        return;

    QString detailText = createModelString(smartphones[index]);
    detailText.append("<br><br>");

    QList<SensorLensPair> pairs = smartphones[index].getMainCams();
    QList<SensorLensPair> selfie = smartphones[index].getSelfieCams();

    QString camText;
    for (int i=0; i<pairs.size(); i++){
        camText.append(QString("<b>Kamera: %1</b><br>").arg(i+1));
        camText.append(createSensorPairString(pairs[i]));
    }

    for (int i=0; i<selfie.size(); i++){
        camText.append(QString("<b>Selfie: %1</b><br>").arg(i+1));
        camText.append(createSensorPairString(selfie[i]));
    }
    phoneDetailsLabel->setText(detailText + camText);
}

void PhoneInfoWidget::highlightCamera(int phoneIndex, SensorLensPair pair){
    phoneSelector->setCurrentIndex(phoneIndex+1);
}

void PhoneInfoWidget::reset(){
    phoneDetailsLabel->setText("");
    phoneSelector->clear();
}
