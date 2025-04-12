#include "smartphonecomparewidget.h"
#include <QHeaderView>
#include <QLabel>
#include <QGridLayout>

phoneCompareWidget::phoneCompareWidget(QWidget* parent)
    : QWidget(parent), comparisonTable(new QTableWidget(this)) {
    setupUI();
}

void phoneCompareWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* comboLayout = new QHBoxLayout;
    for (int i = 0; i < 4; ++i) {
        QComboBox* combo = new QComboBox;
        comboBoxes.append(combo);
        comboLayout->addWidget(combo);
        connect(combo, &QComboBox::currentTextChanged, this, &phoneCompareWidget::fillTable);
    }

    mainLayout->addLayout(comboLayout);

    comparisonTable->setColumnCount(4);
    comparisonTable->setRowCount(standardFocalLengths.size());
    QStringList headers;
    for (int i = 0; i < 4; ++i)
        headers << "Phone " + QString::number(i + 1);
    comparisonTable->setHorizontalHeaderLabels(headers);
    comparisonTable->setVerticalHeaderLabels(
        QStringList() << "15mm" << "24mm" << "35mm" << "65mm" << "90mm" << "120mm" << "240mm");
    comparisonTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    comparisonTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(comparisonTable, &QTableWidget::cellClicked, this, &phoneCompareWidget::onCellClicked);

    mainLayout->addWidget(comparisonTable);

    // Detailanzeige
    detailLabel = new QLabel("Details zur Auswahl", this);
    detailLabel->setWordWrap(true);
    mainLayout->addWidget(detailLabel);
}

void phoneCompareWidget::fillTable() {
    /*
    comparisonTable->clearContents();

    for (int col = 0; col < comboBoxes.size(); ++col) {
        int phoneIndex = comboBoxes[col]->currentIndex();
        if (phoneIndex < 0 || phoneIndex >= smartphones.size())
            continue;

        const Smartphone& phone = smartphones[phoneIndex];
        const auto& pairs = phone.getSensorLensPairs();

        for (int row = 0; row < standardFocalLengths.size(); ++row) {
            int targetFocal = standardFocalLengths[row];

            // Variablen für das beste Sensor-Linsen-Paar und Farbberechnung
            const QPair<CameraSensor, Lens>* bestPair = nullptr;
            double minDelta = 9999;
            bool isNative = false;
            double focalLengthUsed = 0;
            double apertureUsed = 0;
            double sensorAreaUsed = 0;

            // Überprüfe alle Linsen, um die beste Option zu finden
            for (const auto& pair : pairs) {
                double focal = pair.second.focalLengthMin();

                // Exakte Übereinstimmung (native Linse)
                if (qFuzzyCompare(focal, targetFocal)) {
                    bestPair = &pair;
                    isNative = true;
                    focalLengthUsed = focal;
                    apertureUsed = pair.second.apertureMin();
                    sensorAreaUsed = pair.first.sensorArea();
                    break;
                }

                // Nächstkleinere Brennweite finden (gecroppt)
                if (focal < targetFocal && (targetFocal - focal) < minDelta) {
                    bestPair = &pair;
                    minDelta = targetFocal - focal;
                    focalLengthUsed = focal;
                    apertureUsed = pair.second.apertureMin();
                    sensorAreaUsed = pair.first.sensorArea();
                }
            }

            // Falls ein passendes Paar gefunden wurde, fülle die Zelle
            if (bestPair) {
                double cropFactor = (focalLengthUsed > 0) ? targetFocal / focalLengthUsed : 1;
                double effectiveArea = isNative ? sensorAreaUsed : sensorAreaUsed / (cropFactor * cropFactor);
                double effectiveAperture = isNative ? apertureUsed : apertureUsed * cropFactor;
                double performance = effectiveArea * (1.0 / (effectiveAperture * effectiveAperture));

                QTableWidgetItem* item = new QTableWidgetItem(QString::number(performance, 'f', 1));

                // Wenn native Brennweite, setze Hintergrund auf grün, andernfalls Farbverlauf
                if (isNative) {
                    item->setBackground(Qt::green);
                } else {
                    // Farbverlauf von grün nach orange je nach Abweichung
                    int intensity = qMin(255, static_cast<int>((minDelta / targetFocal) * 255));
                    item->setBackground(QColor(255, 255 - intensity, 0));
                    item->setToolTip(QString("Crop: %.1fx\nOrig: %.1fmm\nEffAperture: f/%.2f\nEffArea: %.1f mm²")
                                         .arg(cropFactor, 0, 'f', 2)
                                         .arg(focalLengthUsed)
                                         .arg(effectiveAperture)
                                         .arg(effectiveArea));
                }

                item->setData(Qt::UserRole, QVariant::fromValue(*bestPair));
                comparisonTable->setItem(row, col, item);
            }
        }
    }
*/
}

void phoneCompareWidget::addSmartphone(Smartphone phone){
    smartphones.append(phone);
    updateComparisonTable();
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
