cmake_minimum_required(VERSION 3.5)

project(cpp7_mlp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)

set(PROJECT_SOURCES
    main.cc
    mainwindow.cc
    mainwindow.h
    mainwindow.ui
    painter.h painter.cc
    ../controller/controller.cc ../controller/controller.h
    ../model/model_matrix.cc ../model/model_matrix.h
    ../model/model_graph.cc ../model/model_graph.h
    ../model/s21_matrix_oop.cc ../model/s21_matrix_oop.h
    qcustomplot/qcustomplot.cpp qcustomplot/qcustomplot.h
    qcustomplot_pch.h
    test_dialog.h test_dialog.cc
    resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(cpp7_mlp
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET cpp7_mlp APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(cpp7_mlp SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(cpp7_mlp
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Генерируйте PCH файл
set_source_files_properties(qcustomplot_pch.h PROPERTIES COMPILE_FLAGS "-x c++-header")
target_sources(${PROJECT_NAME} PRIVATE )
# Укажите, что PCH должен использоваться для цели
target_precompile_headers(${PROJECT_NAME} PRIVATE qcustomplot_pch.h)

target_compile_options(${PROJECT_NAME} PRIVATE -O3)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.cpp7_mlp)
endif()
set_target_properties(cpp7_mlp PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS cpp7_mlp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(cpp7_mlp)
endif()
