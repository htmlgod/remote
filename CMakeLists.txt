cmake_minimum_required(VERSION 3.16)
project(rd_management VERSION 1.0 LANGUAGES C CXX)

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

add_executable(rd_management
    common.h
    main.cpp
    remote_management.cpp remote_management.h remote_management.ui
)
target_link_libraries(rd_management PRIVATE
    Qt5::Core
    Qt5::Gui
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(rd_management PRIVATE
        Qt5::Network
        Qt5::Widgets
    )
endif()

install(TARGETS rd_management
    RUNTIME DESTINATION /usr/local/remote_server
)
include(InstallRequiredSystemLibraries)
set(CPACK_DEBIAN_PACKAGE_NAME "remote-server")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "CNIIAG")
include(CPack)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
