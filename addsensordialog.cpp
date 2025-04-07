#include "addsensordialog.h"
#include <QFormLayout>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QComboBox>
#include <QMessageBox>
#include <QRegularExpression>

AddSensorDialog::AddSensorDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Sensor hinzufügen");
    auto* layout = new QFormLayout(this);

    nameEdit = new QLineEdit(this);
    pixelsEdit = new QLineEdit(this);
    pixelSizeEdit = new QLineEdit(this);
    formatComboBox = new QComboBox(this);
    inchSizeEdit = new QLineEdit(this);
    resolutionEdit = new QLineEdit(this);
    yearEdit = new QLineEdit(this);

    // Set validators
    pixelsEdit->setValidator(new QIntValidator(1, 100000, this));
    pixelSizeEdit->setValidator(new QDoubleValidator(0.01, 100.0, 2, this));
    yearEdit->setValidator(new QIntValidator(1900, 2100, this));
    //inchSizeEdit->setValidator(new QRegularExpressionValidator(QRegularExpressionValidator("\\d+\\/?\\d+(\\.\\d+)?"), this)); // For inch size like 1/2.76

    // Format ComboBox
    formatComboBox->addItem("4:3");
    formatComboBox->addItem("16:9");
    formatComboBox->addItem("3:2");
    formatComboBox->addItem("1:1");

    // Set default value
    formatComboBox->setCurrentText("4:3");

    layout->addRow("Name:", nameEdit);
    layout->addRow("Horizontale Pixel:", pixelsEdit);
    layout->addRow("Pixelgröße (µm):", pixelSizeEdit);
    layout->addRow("Inch Size:", inchSizeEdit);
    layout->addRow("Auflösung:", resolutionEdit);
    layout->addRow("Format:", formatComboBox);
    layout->addRow("Erscheinungsjahr:", yearEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [=]() {
        QString cname = nameEdit->text();
        int hpixels = pixelsEdit->text().toInt();

        double pixelSize = pixelSizeEdit->text().replace(",", ".").toDouble();
        QString format = formatComboBox->currentText();
        QString inchSize = inchSizeEdit->text();
        int resolution = resolutionEdit->text().toInt();
        int year = yearEdit->text().toInt();

        CameraSensor Hans = CameraSensor::createCameraSensorFromInchSize("Hans", inchSize, resolution, format);
        CameraSensor Wurst = CameraSensor::createCameraSensorFromPixels("Wurst", hpixels, pixelSize, format);
        double expected = Hans.sensorArea();
        double calculated = Wurst.sensorArea();

        // Allow a 5% deviation
        if (qAbs(expected - calculated) / expected <= 0.05) {
            Hans.setReleaseYear(year);
            Hans.setName(cname);
            Hans.setPixelSize(pixelSize);
            Hans.setHorizontalPixels(hpixels);
            Hans.setVerticalPixels(Wurst.verticalPixels());
            Hans.setHeight(Wurst.height());
            Hans.setWidth(Wurst.width());
            Hans.setResolution(Wurst.resolution());
            createdSensor = Hans;
            accept();
        } else {
            QMessageBox::warning(this, "Fehler", "Die berechneten Werte stimmen nicht überein. Die Abweichung beträgt mehr als 5%.");
        }
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &AddSensorDialog::reject);
}

CameraSensor AddSensorDialog::getSensor() const {
    return createdSensor;
}
