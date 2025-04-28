#include "mainwindow.h"
#include <QApplication>

MainWindow *window = nullptr;

void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("%1").arg(msg);
        if (window){
            //window->printMessage(txt);
        }
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
    case QtInfoMsg:
        txt = QString("%1").arg(msg);
        if(window) {
            //window->printMessage(txt);
        }
        break;
    }
    QApplication::processEvents();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageHandler);
    QCoreApplication::setOrganizationName("SilasManufactures");
    QCoreApplication::setApplicationName("CameraCompare");
    QCoreApplication::setApplicationVersion("1.0 alpha");
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QString("%1 [%2]").arg(a.applicationName()).arg(a.applicationVersion()));
    window = &w;
    w.show();
    return a.exec();
}
