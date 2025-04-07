#include "addlensdialog.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QIntValidator>
#include <QDoubleValidator>

AddLensDialog::AddLensDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Objektiv hinzufügen");

    auto* layout = new QFormLayout(this);

    idEdit = new QLineEdit(this);
    focalMinEdit = new QLineEdit(this);
    focalMaxEdit = new QLineEdit(this);
    apertureMinEdit = new QLineEdit(this);
    apertureMaxEdit = new QLineEdit(this);

    focalMinEdit->setValidator(new QIntValidator(1, 2000, this));
    focalMaxEdit->setValidator(new QIntValidator(1, 2000, this));
    apertureMinEdit->setValidator(new QDoubleValidator(0.7, 32.0, 2, this));
    apertureMaxEdit->setValidator(new QDoubleValidator(0.7, 32.0, 2, this));

    layout->addRow("Name", idEdit);
    layout->addRow("Brennweite min (mm):", focalMinEdit);
    layout->addRow("Brennweite max (mm):", focalMaxEdit);
    layout->addRow("Blende max (größte Öffnung):", apertureMinEdit);
    layout->addRow("Blende min (kleinste Öffnung):", apertureMaxEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [=]() {
        QString id = idEdit->text();
        int fMin = focalMinEdit->text().toInt();
        int fMax = focalMaxEdit->text().toInt();
        double aMin = apertureMinEdit->text().replace(",", ".").toDouble();
        double aMax = apertureMaxEdit->text().replace(",", ".").toDouble();

        // Entscheide Typ je nach Eingabe
        if (fMin > 0 && fMax > 0 && fMin != fMax && aMin != aMax) {
            createdLens = Lens(id, fMin, fMax, aMin, aMax); // Zoom
        } else {
            createdLens = Lens(id, fMin, aMin); // Festbrennweite
        }

        accept();
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &AddLensDialog::reject);
}

Lens AddLensDialog::getLens() const {
    return createdLens;
}
