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

class SmartphoneComparatorWidget : public QWidget {
    Q_OBJECT

public:
    explicit SmartphoneComparatorWidget(QWidget* parent = nullptr);

    void setSmartphones(const QList<Smartphone>& phones);
    void fillTable();
    void updateComparisonTable();

private slots:
    void onCellClicked(int row, int column);

private:
    QTableWidget* comparisonTable;
    QList<Smartphone> smartphones;
    QList<QComboBox*> comboBoxes;
    QVector<int> standardFocalLengths = {15, 24, 35, 65, 90, 120, 240};
    QLabel* detailLabel;

    void setupUI();
};

#endif // SMARTPHONECOMPARATORWIDGET_H
