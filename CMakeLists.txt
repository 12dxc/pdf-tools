cmake_minimum_required(VERSION 3.16)
project(pdf-tools VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    PdfWidgets
    PrintSupport
)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/PdfView.cpp
    src/PdfView.h
    src/TocWidget.cpp
    src/TocWidget.h
    src/SearchWidget.cpp
    src/SearchWidget.h
    src/BookmarkWidget.cpp
    src/BookmarkWidget.h
    resources/resources.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PdfWidgets
    Qt6::PrintSupport
)

target_include_directories(${PROJECT_NAME} PRIVATE src)
