#include "smartphonecomparewidget.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QColor>
#include <QDebug>

SmartphoneComparatorWidget::SmartphoneComparatorWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void SmartphoneComparatorWidget::setupUI() {
    auto* layout = new QVBoxLayout(this);

    auto* phoneSelectorLayout = new QHBoxLayout();
    phoneSelector1 = new QComboBox(this);
    phoneSelector2 = new QComboBox(this);
    phoneSelector3 = new QComboBox(this);
    phoneSelector4 = new QComboBox(this);

    phoneSelectorLayout->addWidget(phoneSelector1);
    phoneSelectorLayout->addWidget(phoneSelector2);
    phoneSelectorLayout->addWidget(phoneSelector3);
    phoneSelectorLayout->addWidget(phoneSelector4);

    layout->addLayout(phoneSelectorLayout);

    comparisonTable = new QTableWidget(this);
    comparisonTable->setRowCount(7); // 7 Brennweiten
    comparisonTable->setColumnCount(4); // Maximal 4 Smartphones
    comparisonTable->setVerticalHeaderLabels({"15mm", "24mm", "35mm", "65mm", "85mm", "120mm", "240mm"});
    layout->addWidget(comparisonTable);

    connect(phoneSelector1, SIGNAL(currentIndexChanged(int)), this, SLOT(updateComparisonTable()));
    connect(phoneSelector2, SIGNAL(currentIndexChanged(int)), this, SLOT(updateComparisonTable()));
    connect(phoneSelector3, SIGNAL(currentIndexChanged(int)), this, SLOT(updateComparisonTable()));
    connect(phoneSelector4, SIGNAL(currentIndexChanged(int)), this, SLOT(updateComparisonTable()));
}

void SmartphoneComparatorWidget::setSmartphones(const QList<Smartphone>& phones) {
    smartphones = phones;
    phoneSelector1->clear();
    phoneSelector2->clear();
    phoneSelector3->clear();
    phoneSelector4->clear();

    for (const Smartphone& phone : smartphones) {
        phoneSelector1->addItem(phone.name());
        phoneSelector2->addItem(phone.name());
        phoneSelector3->addItem(phone.name());
        phoneSelector4->addItem(phone.name());
    }

    // Setze die Horizontal-Header auf aktuelle Smartphone-Namen
    QStringList headers;
    QList<QComboBox*> selectors = {phoneSelector1, phoneSelector2, phoneSelector3, phoneSelector4};
    for (QComboBox* selector : selectors) {
        int index = selector->currentIndex();
        if (index >= 0 && index < smartphones.size()) {
            headers << smartphones[index].name();
        } else {
            headers << "";
        }
    }
    comparisonTable->setHorizontalHeaderLabels(headers);
    updateComparisonTable();
}

void SmartphoneComparatorWidget::updateComparisonTable() {
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 4; ++j) {
            comparisonTable->setItem(i, j, new QTableWidgetItem(""));
        }
    }

    QList<QComboBox*> selectors = {phoneSelector1, phoneSelector2, phoneSelector3, phoneSelector4};
    QList<int> standardFocalLengths = {15, 24, 35, 65, 85, 120, 240};

    QStringList headers;

    for (int j = 0; j < selectors.size(); ++j) {
        int index = selectors[j]->currentIndex();
        if (index < 0 || index >= smartphones.size()) {
            headers << "";
            continue;
        }
        const Smartphone& phone = smartphones[index];
        headers << phone.name();

        for (int i = 0; i < standardFocalLengths.size(); ++i) {
            int focalLength = standardFocalLengths[i];
            bool isNative = false;
            QColor color;
            QString lensInfo = findClosestLensInfo(phone, focalLength, isNative, color);

            QTableWidgetItem* item = new QTableWidgetItem(lensInfo);
            item->setBackground(QBrush(color));
            comparisonTable->setItem(i, j, item);
        }
    }

    comparisonTable->setHorizontalHeaderLabels(headers);
}

QString SmartphoneComparatorWidget::findClosestLensInfo(const Smartphone& phone, int targetFocalLength, bool& isNative, QColor& color) {
    for (const auto& pair : phone.sensorLensPairs()) {
        const CameraSensor& sensor = pair.first;
        const Lens& lens = pair.second;

        if (lens.focalLengthMin() <= targetFocalLength && lens.focalLengthMax() >= targetFocalLength) {
            isNative = true;
            color = Qt::green;
            return QString("%1-%2mm f/%3\n%4MP %5mm")
                .arg(lens.focalLengthMin())
                .arg(lens.focalLengthMax())
                .arg(lens.apertureMax())
                .arg(sensor.resolution() / 1e6)
                .arg(sensor.sensorArea());
        }
    }

    int closestFocalLength = -1;
    const Lens* bestLens = nullptr;
    const CameraSensor* bestSensor = nullptr;
    for (const auto& pair : phone.sensorLensPairs()) {
        const Lens& lens = pair.second;
        if (lens.focalLengthMax() < targetFocalLength && lens.focalLengthMax() > closestFocalLength) {
            closestFocalLength = lens.focalLengthMax();
            bestLens = &lens;
            bestSensor = &pair.first;
        }
    }

    if (bestLens) {
        color = QColor("orange");
        return QString("%1-%2mm f/%3\n%4MP %5mm")
            .arg(bestLens->focalLengthMin())
            .arg(bestLens->focalLengthMax())
            .arg(bestLens->apertureMax())
            .arg(bestSensor->resolution() / 1e6)
            .arg(bestSensor->sensorArea());
    }

    color = Qt::red;
    return "Keine Linse verfügbar";
}
