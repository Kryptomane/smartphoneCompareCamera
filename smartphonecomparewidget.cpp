#include "smartphonecomparewidget.h"
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

    // Spaltenüberschriften
    QStringList colHeaders;
    colHeaders << "Focal"; // Spalte 0
    for (int i = 0; i < 4; ++i)
        colHeaders << ""; // Spalten 1–4: leer für ComboBoxen
    comparisonTable->setHorizontalHeaderLabels(colHeaders);

    // Zeilenbeschriftung (inkl. ComboBox-Zeile + Selfie)
    QStringList rowLabels;
    rowLabels << ""; // Zeile 0 = ComboBox-Zeile
    for (int focal : standardFocalLengths)
        rowLabels << QString::number(focal) + " mm";
    rowLabels << "Selfie";
    comparisonTable->setVerticalHeaderLabels(rowLabels);


    comparisonTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    comparisonTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
                QString lightValue = calculateLightValue(bestPair,requestedFocal);
                QTableWidgetItem* item = new QTableWidgetItem(lightValue);
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
            comparisonTable->setItem(selfieRow, col, item);
        }
    }
}

QString phoneCompareWidget::calculateLightValue(const SensorLensPair pair, int targetFocal) {
    CameraSensor sensor = m_sensorWidget->getCameraByName(pair.sensorName);
    Lens lens = m_lensWidget->getLensById(pair.lensId);

    double focalLength = lens.focalLengthMin();
    double aperture = lens.apertureMin();
    double sensorWidth = sensor.width();
    double sensorHeight = sensor.height();

    double effectiveArea = sensorWidth * sensorHeight;
    bool cropped = false;

    if (targetFocal > 0 && focalLength < targetFocal) {
        double cropFactor = static_cast<double>(targetFocal) / focalLength;
        double croppedWidth = sensorWidth / cropFactor;
        double croppedHeight = sensorHeight / cropFactor;
        effectiveArea = croppedWidth * croppedHeight;
        cropped = true;
    }

    if (aperture <= 0 || effectiveArea <= 0)
        return "N/A";

    // Lichtaufnahme-Index: Fläche geteilt durch quadratische Blendenwirkung
    double lightScore = effectiveArea / (aperture * aperture);

    QString result = QString("%1").arg(lightScore, 0, 'f', 1);
    if (cropped)
        result += " (crop)";
    return result;
}

QList<Smartphone> phoneCompareWidget::getSmartphones()
{
    return smartphones;  // Gibt die interne Liste der Linsen zurück
}

void phoneCompareWidget::onCellClicked(int row, int column) {
    // Erste Zeile (ComboBox) oder erste Spalte (Focal) ignorieren
    if (row == 0 || column == 0)
        return;

    int phoneIndex = comboBoxes[column - 1]->currentIndex(); // column-1 wegen ComboBoxes
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


void phoneCompareWidget::updateComparisonTable() {
    /*
    // Clear previous table contents
    comparisonTable->clearContents();

    // Loop over each combo box and fill in the corresponding column of the table
    for (int col = 0; col < comboBoxes.size(); ++col) {
        int phoneIndex = comboBoxes[col]->currentIndex();
        if (phoneIndex < 0 || phoneIndex >= smartphones.size()) {
            continue;
        }

        const Smartphone& phone = smartphones[phoneIndex];
        const auto& pairs = phone.getSensorLensPairs(); // Get lens-sensor pairs

        // Loop through standard focal lengths
        for (const auto& pair : pairs) {
            double focal = pair.second.focalLengthMin(); // Focal length of the lens
            double aperture = pair.second.apertureMin(); // Aperture of the lens
            double area = pair.first.width() * pair.first.height(); // Sensor area
            double performance = area * (1.0 / (aperture * aperture)); // Performance value

            int row = standardFocalLengths.indexOf(static_cast<int>(focal)); // Find the matching focal length row
            if (row >= 0) {
                // Create a table item and set the performance value with a green background
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(performance, 'f', 1));
                item->setBackground(Qt::green);
                item->setData(Qt::UserRole, QVariant::fromValue(pair)); // Store the lens-sensor pair for reference
                comparisonTable->setItem(row, col, item); // Add the item to the table
            }
        }
    }
*/
}
