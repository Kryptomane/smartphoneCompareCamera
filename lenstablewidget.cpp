#include "lenstablewidget.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QBrush>

LensWidget::LensWidget(QWidget *parent)
    : QWidget(parent),
    lensTable(new QTableWidget(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(lensTable);

    lensTable->setColumnCount(7);  // 6 Spalten: id, type, focalLengthMin, focalLengthMax, apertureMax, apertureMin
    lensTable->setHorizontalHeaderLabels({"ID", "Type", "Focal Length Min", "Focal Length Max", "Aperture Max", "Aperture Min", "Stabi"});
    lensTable->horizontalHeader()->setStretchLastSection(true);
}

void LensWidget::setLenses(const QList<Lens>& lenses)
{
    lensesList = lenses;
    updateTable();
}

void LensWidget::updateTable()
{
    lensTable->setRowCount(lensesList.size());  // Anzahl der Zeilen entspricht der Anzahl der Linsen

    for (int row = 0; row < lensesList.size(); ++row)
    {
        const Lens& lens = lensesList[row];
        lensTable->setItem(row, 0, new QTableWidgetItem(lens.id()));
        lensTable->setItem(row, 1, new QTableWidgetItem(lens.type()));
        lensTable->setItem(row, 2, new QTableWidgetItem(QString::number(lens.focalLengthMin())));
        lensTable->setItem(row, 3, new QTableWidgetItem(QString::number(lens.focalLengthMax())));
        lensTable->setItem(row, 4, new QTableWidgetItem(QString::number(lens.apertureMax())));
        lensTable->setItem(row, 5, new QTableWidgetItem(QString::number(lens.apertureMin())));
        if (lens.stabi() == StabilizationMethod::NoStabilization){
            lensTable->setItem(row, 6, new QTableWidgetItem("No Stabilization"));
        }
        if (lens.stabi() == StabilizationMethod::OIS){
            lensTable->setItem(row, 6, new QTableWidgetItem("OIS"));
        }
        if (lens.stabi() == StabilizationMethod::Gimbal){
            lensTable->setItem(row, 6, new QTableWidgetItem("Gimbal"));
        }
    }
}

QList<Lens> LensWidget::getLenses() const
{
    return lensesList;  // Gibt die interne Liste der Linsen zurück
}

Lens LensWidget::getLensById(const QString& lensId) {
    for (const Lens& lens : std::as_const(lensesList)) {
        if (lens.id() == lensId) {
            return lens;
        }
    }
    // Optional: Fehlerbehandlung oder Dummy-Lens zurückgeben
    qWarning() << "Lens with ID" << lensId << "not found!";
    return Lens(); // Leere/dummy Lens zurückgeben
}

void LensWidget::addLens(Lens temp){
    lensesList.append(temp);
    updateTable();
}

void LensWidget::reset(){
    lensesList.clear();
    updateTable();
}
