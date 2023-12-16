cmake_minimum_required(VERSION 3.16)

project(Mail-Client VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)

if (UNIX)
    list(APPEND CMAKE_PREFIX_PATH /opt/Qt/6.6.1/gcc_64/)
endif (UNIX)

find_package(Qt6 REQUIRED COMPONENTS Widgets Qml Gui WebEngineWidgets)
qt_standard_project_setup()

add_subdirectory(src/lib/json)

if(UNIX)
    qt_add_executable(Mail-Client
        src/gui/ResourcesCompiler.qrc
        src/_bootstrap.cpp
        src/SocketAPI.cpp
        src/test_unit.cpp
        src/SMTPClient.cpp
        src/POP3Client.cpp
        src/gui/ConfigWidget.cpp
        src/gui/AttachmentListWidget.cpp
        src/gui/BoxShadowEffect.cpp
        src/gui/ContentWidget.cpp
        src/gui/MainWindow.cpp
        src/gui/SendMenu.cpp
        src/gui/MailboxMenu.cpp
        src/mime-provider/Base64.cpp
        src/mime-provider/QuotedPrintable.cpp
        src/mime-provider/Parser.cpp
        src/mime-provider/MIMEParser.cpp
        src/mime-provider/MIMEInterface.cpp
        src/local-client/ConfigProvider.cpp
        src/local-client/ClientMailbox.cpp
    )
endif (UNIX)

if(MSVC)
    qt_add_executable(Mail-Client
        src/gui/ResourcesCompiler.qrc
        src/_bootstrap.cpp
        src/SocketAPI.cpp
        src/test_unit.cpp
        src/SMTPClient.cpp
        src/POP3Client.cpp
        src/gui/ConfigWidget.cpp
        src/gui/AttachmentListWidget.cpp
        src/gui/BoxShadowEffect.cpp
        src/gui/ContentWidget.cpp
        src/gui/MainWindow.cpp
        src/gui/SendMenu.cpp
        src/mime-provider/Base64.cpp
        src/mime-provider/QuotedPrintable.cpp
        src/mime-provider/Parser.cpp
        src/mime-provider/MIMEParser.cpp
        src/mime-provider/MIMEInterface.cpp
        src/local-client/ConfigProvider.cpp
        src/local-client/ClientMailbox.cpp
    )
endif (MSVC)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)

if (UNIX)
    target_link_libraries(Mail-Client PRIVATE Qt6::Widgets 
                                            Qt6::Gui
                                            Qt6::WebEngineWidgets
                                            Qt6::Qml nlohmann_json
                        )
    target_include_directories(Mail-Client PUBLIC src/include 
                                                src/lib/json/include
                            )
endif (UNIX)

if (MSVC)
    target_link_libraries(Mail-Client PRIVATE Qt6::Widgets 
                                            Qt6::Gui
                                            Qt6::WebEngineWidgets
                                            Qt6::Qml nlohmann_json
                        )
    
    target_include_directories(Mail-Client PUBLIC src/include 
                                                src/lib/json/include
                            )
endif (MSVC)
set_target_properties(Mail-Client PROPERTIES AUTOMOC ON)

set_target_properties(Mail-Client PROPERTIES
    OUTPUT_NAME "MailClientShip"
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)