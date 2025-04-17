cmake_minimum_required(VERSION 3.19)
project(camera_calculator LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(camera_calculator
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    camerasensor.h camerasensor.cpp
    addsensordialog.h addsensordialog.cpp
    lens.h lens.cpp
    addlensdialog.h addlensdialog.cpp
    smartphone.h smartphone.cpp
    addsmartphonedialog.h addsmartphonedialog.cpp
    smartphonecomparewidget.h smartphonecomparewidget.cpp
    lenstablewidget.h lenstablewidget.cpp
    camerasensortablewidget.h camerasensortablewidget.cpp
    phonedetailwidget.h phonedetailwidget.cpp
)

target_link_libraries(camera_calculator
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS camera_calculator
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET camera_calculator
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
