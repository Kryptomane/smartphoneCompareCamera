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

class phoneCompareWidget : public QWidget {
    Q_OBJECT

public:
    explicit phoneCompareWidget(QWidget* parent = nullptr);
    void setSensorAndLensWidgets(CameraSensorTableWidget* sensorWidget, LensTableWidget* lensWidget);
    void fillTable(int column, const Smartphone& phone);
    void onSmartphoneSelected(int index);
    void updateComparisonTable();
    QList<Smartphone> getSmartphones();
    void addSmartphone(Smartphone phone);
    QString calculateLightValue(SensorLensPair pair, int targetFocal);
private slots:
    void onCellClicked(int row, int column);

private:
    LensTableWidget* m_lensWidget;
    CameraSensorTableWidget* m_sensorWidget;
    QTableWidget* comparisonTable;
    QList<Smartphone> smartphones;
    QList<QComboBox*> comboBoxes;
    QVector<int> standardFocalLengths = {15, 24, 35, 50, 65, 85, 120, 240};
    QLabel* m_detailLabel;
    void setupUI();
};

#endif // SMARTPHONECOMPARATORWIDGET_H
