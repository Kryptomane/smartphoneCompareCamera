// smartphonecomparatorwidget.h

#ifndef SMARTPHONECOMPARATORWIDGET_H
#define SMARTPHONECOMPARATORWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>

#include "smartphone.h"

class SmartphoneComparatorWidget : public QWidget {
    Q_OBJECT

public:
    explicit SmartphoneComparatorWidget(QWidget* parent = nullptr);

    void setSmartphones(const QList<Smartphone>& phones);
    void updateComparisonTable();

private:
    QComboBox* phoneSelector1;
    QComboBox* phoneSelector2;
    QComboBox* phoneSelector3;
    QComboBox* phoneSelector4;

    QTableWidget* comparisonTable;
    QList<Smartphone> smartphones;

    QVector<int> standardFocalLengths = {15, 24, 35, 65, 90, 120, 240};

    void setupUI();
    void fillTable();
    QString findClosestLensInfo(const Smartphone& phone, int targetFocalLength, bool& isNative, QColor& color);
};

#endif // SMARTPHONECOMPARATORWIDGET_H
