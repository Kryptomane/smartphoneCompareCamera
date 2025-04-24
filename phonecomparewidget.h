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
    double effectiveAperture;
    double maxLight;
    QColor cellcolor;
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

public slots:
    void printInfoMessage(const QString &s);

private slots:
    void onCellClicked(int row, int column);


private:
    LensWidget* m_lensWidget;
    SensorWidget* m_sensorWidget;
    QTableWidget* comparisonTable;
    QList<Smartphone> smartphones;
    QList<QComboBox*> comboBoxes;
    QVector<int> standardFocalLengths = {15, 24, 35, 70, 85, 100, 135};
    QLabel* m_detailLabel;
    void setupUI();
};

#endif // SMARTPHONECOMPARATORWIDGET_H
