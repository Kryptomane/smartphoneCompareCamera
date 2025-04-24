#ifndef LENSTABLEWIDGET_H
#define LENSTABLEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QList>
#include "lens.h"  // Assuming that Lens class is already defined elsewhere

class LensWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LensWidget(QWidget *parent = nullptr);
    void setLenses(const QList<Lens>& lenses);  // Setzen der Liste von Linsen
    void updateTable();  // Die Tabelle aktualisieren
    QList<Lens> getLenses() const;  // Getter für die Liste der Linsen
    Lens getLensById(const QString& lensId);
    void addLens(Lens temp);
    void reset();

private:
    QTableWidget* lensTable;  // Tabelle zur Anzeige der Linsen
    QList<Lens> lensesList;   // Liste aller Linsen
};

#endif // LENSTABLEWIDGET_H
