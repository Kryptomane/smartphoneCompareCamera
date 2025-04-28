#include "phonecomparewidget.h"
#include <QHeaderView>
#include <QLabel>
#include <QGridLayout>

CompareWidget::CompareWidget(QWidget* parent)
    : QWidget(parent),
    comparisonTable(new QTableWidget(this))
{
    setupUI();
}

void CompareWidget::setSensorAndLensWidgets(SensorWidget* sensorWidget, LensWidget* lensWidget) {
    m_sensorWidget = sensorWidget;
    m_lensWidget = lensWidget;
}

void CompareWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    int numberPhonesToCompare = 4;
    int totalRows = standardFocalLengths.size() + 1 + 1; //Focals +1 Selfie, +1 ComboBox-Zeile

    // Vergleichstabelle initialisieren
    comparisonTable->setColumnCount(numberPhonesToCompare + 1); // 1x Header + Smartphones
    comparisonTable->setRowCount(totalRows); // ComboBox + Brennweiten + Selfie

    int columnWidth = 160;
    int rowHeight = 45;
    int columnHeaderWidth = 70;
    int rowHeaderHeight = 25;
    int detailLabelHeight = 150;
    int numCols = comparisonTable->columnCount();
    int spacingWidth = 20;

    int tableWidth = columnWidth * numberPhonesToCompare + columnHeaderWidth + spacingWidth;
    int tableHeight = rowHeight * (totalRows - 1) + rowHeaderHeight;

    // Scrollbars deaktivieren
    comparisonTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    comparisonTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Größe fixieren
    comparisonTable->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    comparisonTable->setMinimumSize(tableWidth, tableHeight);
    comparisonTable->setMaximumSize(tableWidth, tableHeight);

    // ComboBoxen hinzufügen
    for (int col = 1; col <= numberPhonesToCompare; ++col) {
        QComboBox* combo = new QComboBox(this);
        combo->addItem("");
        for (const Smartphone& phone : smartphones)
            combo->addItem(phone.name());
        comboBoxes.append(combo);
        comparisonTable->setCellWidget(0, col, combo);
        connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &CompareWidget::onSmartphoneSelected);
    }

    // Tabellenheader ausblenden und Fix-Modus setzen
    comparisonTable->horizontalHeader()->setVisible(false);
    comparisonTable->verticalHeader()->setVisible(false);
    comparisonTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    comparisonTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Feste Spaltenbreiten und Zeilenhöhen setzen
    comparisonTable->setColumnWidth(0, columnHeaderWidth);
    for (int col = 1; col < numCols; ++col)
        comparisonTable->setColumnWidth(col, columnWidth);
    comparisonTable->setRowHeight(0, rowHeaderHeight);
    for (int row = 1; row < totalRows; ++row)
        comparisonTable->setRowHeight(row, rowHeight);

    // Erste Spalte mit Brennweiten + Selfie-Kennzeichnung füllen
    comparisonTable->setItem(0, 0, new QTableWidgetItem("Focal"));
    for (int i = 0; i < standardFocalLengths.size(); ++i) {
        QString value = QString::number(standardFocalLengths[i]) + "mm";
        comparisonTable->setItem(i + 1, 0, new QTableWidgetItem(value));
    }
    comparisonTable->setItem(standardFocalLengths.size() + 1, 0, new QTableWidgetItem("Selfie"));

    // Klick-Handling für Detailanzeige
    connect(comparisonTable, &QTableWidget::cellClicked,
            this, &CompareWidget::onCellClicked);

    // Vergleichstabelle zum Layout hinzufügen
    mainLayout->addWidget(comparisonTable);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setFixedSize(tableWidth, tableHeight+15);
    // Vertikalen Spacer hinzufügen (sorgt dafür, dass das Label oben bleibt)
    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}


void CompareWidget::addSmartphone(Smartphone phone) {
    QList<SensorLensPair> pairs = phone.getMainCams();
    for (int i=0; i<pairs.size(); i++)
    {
        CameraSensor sensor = m_sensorWidget->getCameraByName(pairs[i].sensorName);
        Lens lens = m_lensWidget->getLensById(pairs[i].lensId);
        double fl = lens.focalLengthMin() / sensor.cropFactor();
        double fov = 2.0 * qRadiansToDegrees(qAtan(sensor.diagonal() / (2.0 * fl)));

        if (!CameraSensor::compareValueDeviation(fov,pairs[i].fieldOfView)){
            qWarning() << "Neu berechnet fov" << fov << pairs[i].fieldOfView;
            phone.setFOV(i,fov);
        }
    }
    smartphones.append(phone);
    for (QComboBox* combo : comboBoxes)
    {
        combo->addItem(phone.manufacturer() + " " + phone.name());
    }
}
void CompareWidget::onSmartphoneSelected(int index) {
    QComboBox* senderCombo = qobject_cast<QComboBox*>(sender());
    if (!senderCombo) return;

    int column = comboBoxes.indexOf(senderCombo)+1;

    if (column < 0 || (index == 0 && column !=0) ) {
        // Nur Zellen in Spalte `column` zurücksetzen (ohne Kopfzeile und ohne Spalte 0!)
        for (int row = 1; row < comparisonTable->rowCount(); ++row) {
            QWidget* widget = comparisonTable->cellWidget(row, column);
            if (widget) {
                comparisonTable->removeCellWidget(row, column);
                delete widget;
            }

            QTableWidgetItem* item = comparisonTable->item(row, column);
            if (item) {
                delete item;
            }
            comparisonTable->setItem(row, column, nullptr);
        }
        return;
    }

    // ✅ Absturz vermeiden bei leerer Liste oder falschem Index
    if (index - 1 < 0 || index - 1 >= smartphones.size()) {
        return;
    }

    Smartphone selectedPhone = smartphones.at(index - 1);
    fillTable(column, selectedPhone);
}

QLabel* CompareWidget::createLightInfoItem(LightStruct result) {
    QString htmlText = QString(
                           "<div style='text-align:center;'>"
                           "<span style='font-size:14pt; font-weight:bold;'>L: %1</span><br>"
                           "<span style='font-size:9pt;'>Crop:%2 - maxL:%3</span>"
                           "</div>")
                           .arg(result.lightvalue, 0, 'f', 2)
                           .arg(result.cropfactor, 0, 'f', 2)
                           .arg(result.maxLight, 0, 'f', 1);

    QLabel* label = new QLabel();
    label->setTextFormat(Qt::RichText);
    label->setText(htmlText);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    label->setAutoFillBackground(true);

    QPalette palette = label->palette();
    palette.setColor(QPalette::Window, result.cellcolor);
    label->setPalette(palette);
    label->setBackgroundRole(QPalette::Window); // 🔑 Das sorgt dafür, dass der Hintergrund korrekt dargestellt wird

    return label;
}

void CompareWidget::fillTable(int column, const Smartphone& phone) {
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
                LightStruct result = calculateLightValue(bestPair, requestedFocal);
                QLabel* item = createLightInfoItem(result);
                comparisonTable->setCellWidget(row, col, item);
            }
        }

        SensorLensPair selfiePair("SelfDummy", "SelfDummy",1000);
        bool hasSelfie = false;

        if (!phone.getSelfieCams().isEmpty()) {
            selfiePair = phone.getSelfieCams().first();
            hasSelfie = true;
        }

        if (hasSelfie) {
            Lens selfLens = m_lensWidget->getLensById(selfiePair.lensId);
            LightStruct result = calculateLightValue(selfiePair,selfLens.focalLengthMin());
            QLabel* item = createLightInfoItem(result);
            comparisonTable->setCellWidget(selfieRow, col, item);
        }
    }
}

LightStruct CompareWidget::calculateLightValue(const SensorLensPair pair, int targetFocal) {
    LightStruct lightresult;
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
        return lightresult;

    double lightValue = effectiveArea / (effectiveAperture * effectiveAperture);
    double maxLight = sensorArea / (aperture * aperture);

    if (lens.stabi() == StabilizationMethod::OIS){
        lightValue*=1.6;
        maxLight*=1.6;
    }
    if (lens.stabi() == StabilizationMethod::Gimbal){
        lightValue*=1.8;
        maxLight*=1.8;
    }

    lightresult.lightvalue = lightValue;
    lightresult.cropfactor = cropFactor;
    lightresult.effectiveAperture = effectiveAperture;
    lightresult.effectiveArea = effectiveArea;
    lightresult.maxLight = maxLight;

    //Brennweite der gefundenen Linse

    // Farbliche Markierung: ±3 mm → grün, sonst orange
    if ((qAbs(focalMin - targetFocal) <= 3) || (lens.focalLengthMin()<targetFocal && lens.focalLengthMax()>targetFocal)){
        lightresult.cellcolor = QColor(0, 100, 0);  // hellgrün
    } else {
        lightresult.cellcolor = QColor(255, 140, 0);  // leicht orange
    }
    return lightresult;
}

QList<Smartphone> CompareWidget::getSmartphones()
{
    return smartphones;  // Gibt die interne Liste der Linsen zurück
}

void CompareWidget::onCellClicked(int row, int column) {
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

    emit cameraSelected(phoneIndex, *bestPair);

    if (!bestPair)
        return;
}

void CompareWidget::reset(){
    smartphones.clear();
    for (QComboBox* combo : comboBoxes) {
        combo->clear();
    }
    for (int i =0; i<comboBoxes.length(); i++)
    {
        comboBoxes[i]->addItem("");
    }
}
