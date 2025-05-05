#ifndef SMARTPHONECOMPARATORWIDGET_H
#define SMARTPHONECOMPARATORWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include <QList>
#include <QVector>
#include <QPair>
#include <QVariant>
#include "smartphone.h"
#include "lens.h"
#include "camerasensor.h"
#include "camerasensortablewidget.h"
#include "lenstablewidget.h"

struct LightStruct{
    double lightvalue;
    double cropfactor;
    double effectiveArea;
    double maxLight;
    QColor cellcolor;
    double aperture;
};

class CompareWidget : public QWidget {
    Q_OBJECT

public:
    explicit CompareWidget(QWidget* parent = nullptr);
    void setSensorAndLensWidgets(SensorWidget* sensorWidget, LensWidget* lensWidget);
    void fillTable(int column, const Smartphone& phone);
    void onSmartphoneSelected(int index);
    QList<Smartphone> getSmartphones();
    void addSmartphone(Smartphone phone);
    LightStruct calculateLightValue(SensorLensPair pair, int targetFocal);
    QLabel* createLightInfoItem(LightStruct result);
    void reset();

private slots:
    void onCellClicked(int row, int column);

signals:
    void cameraSelected(int phoneIndex, SensorLensPair pair);

private:
    LensWidget* m_lensWidget;
    SensorWidget* m_sensorWidget;
    QTableWidget* comparisonTable;
    QList<Smartphone> smartphones;
    QList<QComboBox*> comboBoxes;
    QVector<int> standardFocalLengths = {15, 25, 35, 70, 85, 100, 135,240};
    void setupUI();
};

#endif // SMARTPHONECOMPARATORWIDGET_H
