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

    // Obere Combobox-Reihe
    QHBoxLayout* comboLayout = new QHBoxLayout;
    for (int i = 0; i < 4; ++i) {
        QComboBox* combo = new QComboBox(this);
        combo->addItem(""); // leeres Element
        comboBoxes.append(combo);
        comboLayout->addWidget(combo);
        connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &phoneCompareWidget::onSmartphoneSelected);
    }
    mainLayout->addLayout(comboLayout);

    // Vergleichstabelle vorbereiten
    int mainCameraRows = standardFocalLengths.size();
    int totalRows = mainCameraRows + 1; // +1 für Selfie-Cam

    comparisonTable->setRowCount(totalRows);
    comparisonTable->setColumnCount(4); // 4 Smartphones auswählbar
    comparisonTable->setHorizontalHeaderLabels(QStringList{"A", "B", "C", "D"});

    QStringList rowLabels;
    for (int focal : standardFocalLengths) {
        rowLabels << QString::number(focal) + " mm";
    }
    rowLabels << "Selfie"; // letzte Zeile für Selfie-Cam
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
    int rowCount = standardFocalLengths.size();
    if (comparisonTable->rowCount() < rowCount)
        comparisonTable->setRowCount(rowCount);

    const QList<SensorLensPair>& mainCams = phone.getMainCams();

    for (int row = 0; row < rowCount; ++row) {
        int targetFocal = standardFocalLengths[row];
        const SensorLensPair* bestPair = nullptr;
        double bestFocal = -1;

        // Suche nach der besten Kamera ≤ targetFocal
        for (const SensorLensPair& pair : mainCams) {
            Lens temp = m_lensWidget->getLensById(pair.lensId);

            double focal = temp.focalLengthMin();
            if (focal <= targetFocal && focal > bestFocal) {
                bestFocal = focal;
                bestPair = &pair;
            }
        }

        if (bestPair) {
            QString value = calculateLightValue(*bestPair); // <-- muss existieren
            QTableWidgetItem* item = new QTableWidgetItem(value);
            comparisonTable->setItem(row, column, item);
        } else {
            QTableWidgetItem* emptyItem = new QTableWidgetItem("-");
            comparisonTable->setItem(row, column, emptyItem);
        }

        // Zeilenbeschriftung (optional, falls nicht bereits gesetzt)
        comparisonTable->setVerticalHeaderItem(row,
                                               new QTableWidgetItem(QString("%1 mm").arg(targetFocal)));
    }
}

QString phoneCompareWidget::calculateLightValue(SensorLensPair pair){
    QString temp = QString("%1%2").arg(pair.sensorName).arg(pair.lensId);
    return temp;
}

QList<Smartphone> phoneCompareWidget::getSmartphones()
{
    return smartphones;  // Gibt die interne Liste der Linsen zurück
}

void phoneCompareWidget::onCellClicked(int row, int column) {
    /*
    QTableWidgetItem* item = comparisonTable->item(row, column);
    if (!item)
        return;

    int phoneIndex = comboBoxes[column]->currentIndex();
    if (phoneIndex < 0 || phoneIndex >= smartphones.size())
        return;

    const Smartphone& phone = smartphones[phoneIndex];
    const auto& pairs = phone.getSensorLensPairs();
    const auto& fovs = phone.getFieldOfViews();
    const auto& resolutions = phone.getAngularResolutions();

    double focal = standardFocalLengths[row];

    for (int i = 0; i < pairs.size(); ++i) {
        if (qFuzzyCompare(pairs[i].second.focalLengthMin(), focal)) {
            const CameraSensor& sensor = pairs[i].first;
            const Lens& lens = pairs[i].second;

            QString detailText = QString(
                                     "<b>Brennweite:</b> %1 mm<br>"
                                     "<b>Blende:</b> f/%2<br>"
                                     "<b>Sensorgröße:</b> %3 mm x %4 mm<br>"
                                     "<b>Auflösung:</b> %5 MP<br>"
                                     "<b>Winkelauflösung:</b> %6 px/Grad<br>"
                                     "<b>Bildwinkel:</b> %7 °")
                                     .arg(lens.focalLengthMin())
                                     .arg(lens.apertureMin())
                                     .arg(sensor.width())
                                     .arg(sensor.height())
                                     .arg(sensor.resolution())
                                     .arg(resolutions.value(i, 0.0), 0, 'f', 2)
                                     .arg(fovs.value(i, 0.0), 0, 'f', 2);

            detailLabel->setText(detailText);
            return;
        }
    }
    */
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
