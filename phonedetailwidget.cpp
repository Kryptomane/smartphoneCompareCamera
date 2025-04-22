#include "phonedetailwidget.h"
#include <QVBoxLayout>

phoneDetailWidget::phoneDetailWidget(QWidget* parent)
    : QWidget(parent), phoneSelector(new QComboBox(this)), phoneDetailsLabel(new QLabel(this)) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(phoneSelector);
    layout->addWidget(phoneDetailsLabel);

    phoneDetailsLabel->setWordWrap(true);

    // Vertikalen Spacer hinzufügen (sorgt dafür, dass das Label oben bleibt)
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    connect(phoneSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &phoneDetailWidget::onSmartphoneSelected);
}

void phoneDetailWidget::setSmartphones(const QList<Smartphone>& phones) {
    smartphones = phones;
    phoneSelector->clear();
    for (const Smartphone& phone : smartphones) {
        phoneSelector->addItem(phone.name());
    }
}

void phoneDetailWidget::onSmartphoneSelected(int index) {
    if (index < 0 || index >= smartphones.size())
        return;

    const Smartphone& phone = smartphones[index];

    QString detailText = QString(
                             "<b>Name:</b> %1<br>"
                             "<b>Hersteller:</b> %2<br>"
                             "<b>Release:</b> %3<br>"
                             "<b>Display:</b> %4 Zoll<br>"
                             "<b>Akku:</b> %5 mAh<br>"
                             "<b>Anzahl Hauptkameras:</b> %6<br>"
                             "<b>Anzahl Selfie-Kameras:</b> %7")
                             .arg(phone.name())
                             .arg(phone.manufacturer())
                             .arg(phone.releaseYear())
                             .arg(phone.displaySize(), 0, 'f', 1)
                             .arg(phone.batteryCapacity())
                             .arg(phone.getMainCams().size())
                             .arg(phone.getSelfieCams().size());

    phoneDetailsLabel->setText(detailText);
}
