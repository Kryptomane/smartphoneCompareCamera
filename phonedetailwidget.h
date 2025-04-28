#ifndef SMARTPHONEDETAILSWIDGET_H
#define SMARTPHONEDETAILSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include "smartphone.h"

class PhoneInfoWidget : public QWidget {
    Q_OBJECT

public:
    explicit PhoneInfoWidget(QWidget* parent = nullptr);
    void setSensorAndLensWidgets(SensorWidget* sensorWidget, LensWidget* lensWidget);
    void setSmartphones(const QList<Smartphone>& phones);
    QString createModelString(const Smartphone& phone);
    QString createSensorPairString(SensorLensPair pair);
    void reset();

public slots:
    void highlightCamera(int phoneIndex, SensorLensPair pair);

private slots:
    void onSmartphoneSelected(int index);

private:
    QComboBox* phoneSelector;
    QLabel* phoneDetailsLabel;
    QList<Smartphone> smartphones;
    LensWidget* m_lensWidget;
    SensorWidget* m_sensorWidget;
};

#endif // SMARTPHONEDETAILSWIDGET_H
