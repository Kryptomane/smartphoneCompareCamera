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
    void setSmartphones(const QList<Smartphone>& phones);
    void reset();

private slots:
    void onSmartphoneSelected(int index);

private:
    QComboBox* phoneSelector;
    QLabel* phoneDetailsLabel;
    QList<Smartphone> smartphones;
};

#endif // SMARTPHONEDETAILSWIDGET_H
