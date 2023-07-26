cmake_minimum_required(VERSION 3.16)
project(remote-server VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Network Widgets)

configure_file(common.h.in common.h)

add_executable(${PROJECT_NAME}
    main.cpp
    remote_management.cpp remote_management.ui
)
target_include_directories(${PROJECT_NAME} PUBLIC 
    "${PROJECT_BINARY_DIR}"
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Network
        Qt5::Widgets
    )
endif()

include(installing.cmake)
include(debpackaging.cmake)
