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

class phoneCompareWidget : public QWidget {
    Q_OBJECT

public:
    explicit phoneCompareWidget(QWidget* parent = nullptr);

    void addSmartphone(Smartphone phone);
    void fillTable();
    void updateComparisonTable();
    QList<Smartphone> getSmartphones();
private slots:
    void onCellClicked(int row, int column);

private:
    QTableWidget* comparisonTable;
    QList<Smartphone> smartphones;
    QList<QComboBox*> comboBoxes;
    QVector<int> standardFocalLengths = {15, 24, 35, 50, 65, 85, 120, 240};
    QLabel* detailLabel;
    void setupUI();
};

#endif // SMARTPHONECOMPARATORWIDGET_H
