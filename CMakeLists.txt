cmake_minimum_required(VERSION 3.16)

project(CrossPlatform_Game_Park_Out VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_GENERATE_QMLLS_INI ON)
find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appCrossPlatform_Game_Park_Out
    gamecontroller.cpp
    gamecontroller.h
    main.cpp
)

qt_add_qml_module(appCrossPlatform_Game_Park_Out
    URI CrossPlatform_Game_Park_Out
    QML_FILES
        Main.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appCrossPlatform_Game_Park_Out PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appCrossPlatform_Game_Park_Out
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appCrossPlatform_Game_Park_Out
    PRIVATE Qt6::Quick
            Qt6::QuickControls2
)

install(TARGETS appCrossPlatform_Game_Park_Out
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
