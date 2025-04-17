#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "smartphonecomparewidget.h"
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

    LensTableWidget* getLensesWidget();

private slots:
    void addSensor();
    void addLens();
    void addSmartphone();

signals:
    void printMessage(const QString &s);

private:
    AddLensDialog* lensDialog;
    LensTableWidget* lensTableWidget;
    AddSensorDialog* sensorDialog;
    CameraSensorTableWidget* cameraSensorTableWidget;
    AddSmartphoneDialog* smartphoneDialog;
    phoneCompareWidget* comparatorWidget;
    SmartphoneDetailsWidget* smartphoneDetailsWidget;

    QString getDatabasePath(const QString& fileName);
    QJsonArray getJson(QString filePath);
    void loadDatabases();
    void updateSensorAndLensLists();
    void exportAllDataToJson();
};

#endif // MAINWINDOW_H
