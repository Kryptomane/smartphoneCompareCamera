#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "phonecomparewidget.h"
#include "smartphone.h"
#include "lens.h"
#include "camerasensor.h"
#include "addlensdialog.h"
#include "addsensordialog.h"  // Dialog für Sensoren
#include "addsmartphonedialog.h"
#include "camerasensortablewidget.h"
#include "lenstablewidget.h"
#include "phonedetailwidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    LensWidget* getLensesWidget();

private slots:
    void addSensor();
    void addLens();
    void addSmartphone();

signals:
    void printMessage(const QString &s);

private:
    AddLensDialog* pLensDialog;
    LensWidget* pLensWidget;
    AddSensorDialog* pSensorDialog;
    SensorWidget* pSensorWidget;
    AddSmartphoneDialog* pSmartphoneDialog;
    CompareWidget* pCompareWidget;
    PhoneInfoWidget* pPhoneInfoWidget;

    void setDatabaseDirectory(const QString& path);
    QString getDatabasePath(const QString& fileName);
    void defaultDatabasePath();
    QJsonArray getJson(QString filePath);
    void loadDatabases();
    void updateSensorAndLensLists();
    void exportAllDataToJson();
    void reset();

    QLineEdit* dbPathLineEdit;
    QString currentDatabaseDirectory;
};

#endif // MAINWINDOW_H
