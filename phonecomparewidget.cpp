#include "phonecomparewidget.h"
#include <QHeaderView>
#include <QLabel>
#include <QGridLayout>

phoneCompareWidget::phoneCompareWidget(QWidget* parent)
    : QWidget(parent),
    comparisonTable(new QTableWidget(this)),
    m_detailLabel(new QLabel(this))
{
    setupUI();
}

void phoneCompareWidget::setSensorAndLensWidgets(CameraSensorTableWidget* sensorWidget, LensTableWidget* lensWidget) {
    m_sensorWidget = sensorWidget;
    m_lensWidget = lensWidget;
}

void phoneCompareWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    int mainCameraRows = standardFocalLengths.size();
    int totalRows = mainCameraRows + 1 + 1; // +1 Selfie, +1 ComboBox-Zeile

    // Vergleichstabelle initialisieren
    comparisonTable->setColumnCount(5); // 1x Focal + 4 Smartphones
    comparisonTable->setRowCount(standardFocalLengths.size() + 2); // +1 für ComboBox-Zeile, +1 für Selfie

    int columnWidth = 180;  // z.B. 150px pro Spalte
    int rowHeight = 40;     // z.B. 40px pro Zeile
    int numCols = 5;        // Focal + 4 Smartphones
    int numRows = standardFocalLengths.size() + 2; // ComboBox + Brennweiten + Selfie

    int tableWidth = columnWidth * numCols;
    int tableHeight = rowHeight * numRows;

    this->setFixedSize(tableWidth, tableHeight + 150); // + etwas extra für DetailLabel

    for (int col = 1; col <= 4; ++col) {
        QComboBox* combo = new QComboBox(this);
        combo->addItem("");
        for (const Smartphone& phone : smartphones)
            combo->addItem(phone.name());

        comboBoxes.append(combo);
        comparisonTable->setCellWidget(0, col, combo);

        connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &phoneCompareWidget::onSmartphoneSelected);
    }
    comparisonTable->horizontalHeader()->setVisible(false);
    comparisonTable->verticalHeader()->setVisible(false);

    comparisonTable->setItem(0, 0, new QTableWidgetItem("Focal"));
    for (int i=1; i<=standardFocalLengths.size();i++){
        QString value = QString::number(standardFocalLengths[i-1])+"mm";
        comparisonTable->setItem(i, 0, new QTableWidgetItem(value));
    }
    comparisonTable->setItem(standardFocalLengths.size() + 1, 0, new QTableWidgetItem("Selfie"));
    connect(comparisonTable, &QTableWidget::cellClicked,
            this, &phoneCompareWidget::onCellClicked);

    mainLayout->addWidget(comparisonTable);

    // Detailbereich unten
    m_detailLabel->setText("Details erscheinen hier...");
    m_detailLabel->setWordWrap(true);
    mainLayout->addWidget(m_detailLabel);
}

void phoneCompareWidget::addSmartphone(Smartphone phone) {
    smartphones.append(phone);
    for (QComboBox* combo : comboBoxes) {
        combo->addItem(phone.name()); // Smartphone::getName() muss existieren
    }
}
void phoneCompareWidget::onSmartphoneSelected(int index) {
    QComboBox* senderCombo = qobject_cast<QComboBox*>(sender());
    if (!senderCombo) return;

    int column = comboBoxes.indexOf(senderCombo);
    if (column < 0 || index == 0) {
        // Leeres Feld gewählt → Spalte ggf. leeren
        comparisonTable->clearContents();
        return;
    }

    Smartphone selectedPhone = smartphones.at(index - 1); // -1 wegen leerem Element
    fillTable(column, selectedPhone);
}

void phoneCompareWidget::fillTable(int column, const Smartphone& phone) {
    int comboRow = 0;
    int selfieRow = comparisonTable->rowCount() - 1;

    // Leere alle Zellen ab Zeile 1
    for (int row = 1; row < comparisonTable->rowCount(); ++row) {
        for (int col = 1; col <= 4; ++col) {
            comparisonTable->setItem(row, col, new QTableWidgetItem(""));
        }
    }

    for (int col = 1; col <= 4; ++col) {
        QComboBox* combo = comboBoxes[col - 1];
        int index = combo->currentIndex() - 1; // -1 weil leeres Element an erster Stelle

        if (index < 0 || index >= smartphones.size())
            continue;

        const Smartphone& phone = smartphones[index];

        // MainCam-Zeilen (Zeile 1 bis N-2)
        for (int row = 1; row <= standardFocalLengths.size(); ++row) {
            int requestedFocal = standardFocalLengths[row - 1];

            SensorLensPair bestPair("dummy", "dummy", 100);
            int bestFocal = -1;
            bool hasMatch = false;

            for (const SensorLensPair& pair : phone.getMainCams()) {
                Lens temp = m_lensWidget->getLensById(pair.lensId);
                int focal = temp.focalLengthMin();

                if (focal <= requestedFocal && focal > bestFocal) {
                    bestFocal = focal;
                    bestPair = pair;
                    hasMatch = true;
                }
            }
            if (hasMatch) {
                QString lightValue = calculateLightValue(bestPair, requestedFocal);
                QTableWidgetItem* item = new QTableWidgetItem(lightValue);

                // Brennweite der gefundenen Linse
                Lens lens = m_lensWidget->getLensById(bestPair.lensId);
                int lensFocal = lens.focalLengthMin();

                // Farbliche Markierung: ±3 mm → grün, sonst orange
                if ((qAbs(lensFocal - requestedFocal) <= 3) || (lens.focalLengthMin()<requestedFocal && lens.focalLengthMax()>requestedFocal)){
                    item->setBackground(QBrush(QColor(180, 255, 180)));  // hellgrün
                } else {
                    item->setBackground(QBrush(QColor(255, 220, 180)));  // leicht orange
                }
                comparisonTable->setItem(row, col, item);
            }
        }

        SensorLensPair selfiePair("SelfDummy", "SelfDummy",1000);
        bool hasSelfie = false;

        if (!phone.getSelfieCams().isEmpty()) {
            selfiePair = phone.getSelfieCams().first();
            hasSelfie = true;
        }

        if (hasSelfie) {
            QString lightValue = calculateLightValue(selfiePair,24);
            QTableWidgetItem* item = new QTableWidgetItem(lightValue);
            item->setBackground(QBrush(QColor(180, 255, 180)));  // hellgrün
            comparisonTable->setItem(selfieRow, col, item);
        }
    }
}

/*
QString phoneCompareWidget::calculateLightValue(const SensorLensPair pair, int targetFocal) {
    CameraSensor sensor = m_sensorWidget->getCameraByName(pair.sensorName);
    Lens lens = m_lensWidget->getLensById(pair.lensId);

    if (targetFocal > lens.focalLengthMax()){
        double cropFactor = targetFocal / lens.focalLengthMin();
        double effectiveArea = sensor.sensorArea() / (cropFactor * cropFactor);
        double effectiveAperture = lens.apertureMin() * cropFactor;
        double lightValue = effectiveArea / std::pow(effectiveAperture, 2);
        QString result = QString("%1").arg(lightValue, 0, 'f', 1);
        result += " (crop)";
        return result;
    } else {
        double cropFactor = targetFocal / lens.focalLengthMin();
        double apertureFactor = (lens.apertureMax()-lens.apertureMin())/cropFactor;
        qInfo() << cropFactor << apertureFactor;
        double effectiveAperture = lens.apertureMin() * apertureFactor;
        double lightValue = sensor.sensorArea() / std::pow(effectiveAperture, 2);
        QString result = QString("%1").arg(lightValue, 0, 'f', 1);
        return result;
    }
}*/

QString phoneCompareWidget::calculateLightValue(const SensorLensPair pair, int targetFocal) {
    // --- Sensor laden ---
    CameraSensor sensor = m_sensorWidget->getCameraByName(pair.sensorName);
    double sensorWidth = sensor.width();
    double sensorHeight = sensor.height();
    double sensorArea = sensorWidth * sensorHeight;

    // --- Linse laden ---
    Lens lens = m_lensWidget->getLensById(pair.lensId);
    double focalMin = lens.focalLengthMin();
    double focalMax = lens.focalLengthMax();
    double apertureMin = lens.apertureMin();
    double apertureMax = lens.apertureMax();

    // --- Brennweite bestimmen ---
    double actualFocal = focalMin;
    double aperture = apertureMin;

    if (focalMin != focalMax) {
        // Zoomlinse: Interpolieren je nach targetFocal
        double t = 0.0;
        if (targetFocal > focalMin && focalMax > focalMin)
            t = qBound(0.0, (targetFocal - focalMin) / (focalMax - focalMin), 1.0);
        actualFocal = focalMin + t * (focalMax - focalMin);
        aperture = apertureMin + t * (apertureMax - apertureMin);
    }

    // --- Crop berechnen ---
    double cropFactor = 1.0;
    if (actualFocal < targetFocal) {
        cropFactor = static_cast<double>(targetFocal) / actualFocal;
    }

    // --- effektive Werte ---
    double effectiveAperture = aperture * cropFactor;
    double effectiveArea = sensorArea / (cropFactor * cropFactor);

    // --- Lichtwert berechnen ---
    if (effectiveAperture <= 0.01) // Vermeide Division durch 0
        return "n/a";

    double lightValue = effectiveArea / (effectiveAperture * effectiveAperture);

    // --- Formatierung für Anzeige ---
    QString result = QString("LW: %1\nf/%2\n%3 mm²")
                         .arg(lightValue, 0, 'f', 2)
                         .arg(effectiveAperture, 0, 'f', 1)
                         .arg(effectiveArea, 0, 'f', 1);

    return result;
}




QList<Smartphone> phoneCompareWidget::getSmartphones()
{
    return smartphones;  // Gibt die interne Liste der Linsen zurück
}

void phoneCompareWidget::printInfoMessage(const QString &s){
    m_detailLabel->setText(s);
}

void phoneCompareWidget::onCellClicked(int row, int column) {
    // Erste Zeile (ComboBox) oder erste Spalte (Focal) ignorieren
    if (row == 0 || column == 0)
        return;

    int phoneIndex = comboBoxes[column-1]->currentIndex()-1; // column-1 wegen ComboBoxes und -1 wegen erster Eintrag ist --
    if (phoneIndex < 0 || phoneIndex >= smartphones.size())
        return;

    const Smartphone& phone = smartphones[phoneIndex];
    const QList<SensorLensPair>& camList = (row == comparisonTable->rowCount() - 1)
                                               ? phone.getSelfieCams()
                                               : phone.getMainCams();

    int requestedFocal = (row == comparisonTable->rowCount() - 1)
                             ? -1 // Selfie-Zeile → keine Focal-Vorgabe
                             : standardFocalLengths[row - 1];

    const SensorLensPair* bestPair = nullptr;
    int bestFocal = -1;

    for (const SensorLensPair& pair : camList) {
        Lens lens = m_lensWidget->getLensById(pair.lensId);
        int focal = lens.focalLengthMin();
        if (requestedFocal == -1 || (focal <= requestedFocal && focal > bestFocal)) {
            bestFocal = focal;
            bestPair = &pair;
        }
    }

    if (!bestPair)
        return;

    CameraSensor sensor = m_sensorWidget->getCameraByName(bestPair->sensorName);
    Lens lens = m_lensWidget->getLensById(bestPair->lensId);

    double angle = bestPair->fieldOfView;
    double pxd = 100;

    QString detailText = QString(
                             "<b>Smartphone:</b> %1<br>"
                             "<b>Kameratyp:</b> %2<br>"
                             "<b>Brennweite:</b> %3 mm<br>"
                             "<b>Blende:</b> f/%4<br>"
                             "<b>Sensorgröße:</b> %5 mm × %6 mm<br>"
                             "<b>Auflösung:</b> %7 MP<br>"
                             "<b>Bildwinkel:</b> %8°<br>"
                             "<b>Winkelauflösung:</b> %9 px/°")
                             .arg(phone.name())
                             .arg((row == comparisonTable->rowCount() - 1) ? "Selfie" : "Main")
                             .arg(lens.focalLengthMin())
                             .arg(lens.apertureMin())
                             .arg(sensor.width())
                             .arg(sensor.height())
                             .arg(sensor.resolution())
                             .arg(angle, 0, 'f', 2)
                             .arg(pxd, 0, 'f', 2);

    m_detailLabel->setText(detailText);
}
