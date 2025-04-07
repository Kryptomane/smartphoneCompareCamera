cmake_minimum_required(VERSION 3.19)
project(CameraCalculator LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(CameraCalculator
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    camerasensor.h camerasensor.cpp
    addsensordialog.h addsensordialog.cpp
    lens.h lens.cpp
    addlensdialog.h addlensdialog.cpp
    smartphone.h smartphone.cpp
    addsmartphonedialog.h addsmartphonedialog.cpp

    smartphonecomparewidget.h smartphonecomparewidget.cpp
    lenstablewidget.h lenstablewidget.cpp
    camerasensortablewidget.h camerasensortablewidget.cpp
)

target_link_libraries(CameraCalculator
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS CameraCalculator
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CameraCalculator
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
