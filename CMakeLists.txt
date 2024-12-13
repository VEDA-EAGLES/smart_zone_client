cmake_minimum_required(VERSION 3.16)

project(smart_zone_client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network Charts Multimedia MultimediaWidgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network Charts Multimedia MultimediaWidgets Concurrent)

# gstreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED 
    gstreamer-1.0
    gstreamer-video-1.0
    gstreamer-app-1.0
)

# GStreamer 설정
include_directories(${GSTREAMER_INCLUDE_DIRS})
link_directories(${GSTREAMER_LIBRARY_DIRS})

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

include_directories(
    ${CMAKE_SOURCE_DIR}/inc
)

set(PROJECT_SOURCES
    src/mainwindow.cpp
    src/httpclient.cpp
    src/gststreamthread.cpp
    src/streamdisplay.cpp
    src/areawidget.cpp
    src/drawwidget.cpp
    src/graphdisplay.cpp
    src/sankeydiagram.cpp
)

set(PROJECT_HEADERS
    inc/area.h inc/camera.h inc/data.h
    inc/mainwindow.h
    inc/httpclient.h
    inc/gststreamthread.h
    inc/streamdisplay.h
    inc/areawidget.h
    inc/drawwidget.h
    inc/graphdisplay.h
    inc/sankeydiagram.h
)

set(PROJECT_UI
    ui/mainwindow.ui
    ui/streamdisplay.ui
    ui/areawidget.ui
    ui/drawwidget.ui
    ui/graphdisplay.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(smart_zone_client
        MANUAL_FINALIZATION
        main.cpp
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_UI}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET VEDA_Qt_blog_client APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(smart_zone_client SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(smart_zone_client
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(smart_zone_client PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    ${GSTREAMER_LIBRARIES}
)

set_target_properties(smart_zone_client PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS smart_zone_client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(smart_zone_client)
endif()
