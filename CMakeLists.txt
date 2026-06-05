cmake_minimum_required(VERSION 3.16)
project(CrossPlatform_Game_Park_Out VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Concurrent)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appCrossPlatform_Game_Park_Out
    main.cpp
    gamecontroller.h
    gamecontroller.cpp
)

qt_add_qml_module(appCrossPlatform_Game_Park_Out
    URI CrossPlatform_Game_Park_Out
    VERSION 1.0
    QML_FILES Main.qml
)

target_link_libraries(appCrossPlatform_Game_Park_Out PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Concurrent
)