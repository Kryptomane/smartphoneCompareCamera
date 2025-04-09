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

private:
    SmartphoneComparatorWidget* comparatorWidget;
    QList<Smartphone> smartphones;
    QListWidget* sensorListWidget;
    QListWidget* lensListWidget;
    AddLensDialog* lensDialog;
    AddSensorDialog* sensorDialog;
    AddSmartphoneDialog* smartphoneDialog;
    CameraSensorTableWidget* cameraSensorTableWidget;
    LensTableWidget* lensTableWidget;

    void loadDataFromJson();
    void saveDataToJson();
    void loadLensData();
    void loadCameraSensors();
    void loadSmartPhones();
    void updateSensorAndLensLists();
};

#endif // MAINWINDOW_H
