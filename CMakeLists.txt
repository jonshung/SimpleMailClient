cmake_minimum_required(VERSION 3.16)

project(Mail-Client VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)

#   Qt include
list(APPEND CMAKE_PREFIX_PATH /opt/Qt/6.6.1/gcc_64/)

find_package(Qt6 REQUIRED COMPONENTS Widgets Qml Gui WebEngineWidgets)
qt_standard_project_setup()

add_subdirectory(src/lib/json)

qt_add_executable(Mail-Client
    src/gui/ResourcesCompiler.qrc
    src/_bootstrap.cpp
    src/SocketAPI.cpp
    src/test_unit.cpp
    src/SMTPClient.cpp
    src/POP3Client.cpp
    src/gui/AttachmentListWidget.cpp
    src/gui/BoxShadowEffect.cpp
    src/gui/ContentWidget.cpp
    src/gui/MainWindow.cpp
    src/gui/SendMenu.cpp
    src/gui/MailboxMenu.cpp
    src/mime-provider/Encoder.cpp
    src/mime-provider/MIMEParser.cpp
    src/mime-provider/MIMEInterface.cpp
    src/local-client/ClientMailbox.cpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)

target_link_libraries(Mail-Client PRIVATE Qt6::Widgets 
                                          Qt6::Gui
                                          Qt6::WebEngineWidgets
                                          Qt6::Qml nlohmann_json
                                          /usr/local/lib/libmimetic.a
                     )
target_include_directories(Mail-Client PUBLIC src/include 
                                              src/lib/json/include
                                              /usr/local/include/mimetic
                          )
set_target_properties(Mail-Client PROPERTIES AUTOMOC ON)

set_target_properties(Mail-Client PROPERTIES
    OUTPUT_NAME "MailClientShip"
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)