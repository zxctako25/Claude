cmake_minimum_required(VERSION 3.16)
project(Claude VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Svg)

if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/claude.rc")
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/claude.rc")
else()
    set(APP_ICON_RESOURCE "")
endif()

qt_add_executable(Claude WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    animatedbackground.cpp
    animatedbackground.h
    chatbubble.cpp
    chatbubble.h
    chatitemwidget.cpp
    chatitemwidget.h
    modelselectordialog.cpp
    modelselectordialog.h
    ${APP_ICON_RESOURCE}
)

target_include_directories(Claude PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Claude PRIVATE Qt6::Widgets Qt6::Network Qt6::Svg)

add_custom_command(TARGET Claude POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/claude.png"
        "$<TARGET_FILE_DIR:Claude>/claude.png"
)
