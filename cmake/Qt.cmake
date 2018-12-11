set(QT_REQUIRED
Qt5Core
Qt5Gui
Qt5Network
Qt5Positioning
Qt5PrintSupport
Qt5Qml
Qt5Quick
Qt5QuickWidgets
Qt5WebChannel
Qt5WebEngine
Qt5WebEngineCore
Qt5WebEngineWidgets
Qt5Widgets
)

set(QT_DEPS
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_AUTOMOC ON)

if (MSVC)
    set(QT_PATH "C:\\Qt\\5.10.1\\msvc2015_64\\")
    set(CMAKE_PREFIX_PATH ${QT_PATH})
    link_directories("${QT_PATH}\\lib")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    set(QT_USED ${QT_REQUIRED} ${QT_DEPS})
    foreach(QTX ${QT_USED})
        message(STATUS "---- Copying Qt5 libraries to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ----")
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(FILE_PATH ${QT_PATH}/bin/${QTX}d.dll)
        endif ()
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            set(FILE_PATH ${QT_PATH}/bin/${QTX}.dll)
        endif ()
        message(STATUS "  ${FILE_PATH}")
        file(COPY ${FILE_PATH}
                DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        )
    endforeach()
endif (MSVC)

foreach(QTX ${QT_REQUIRED})
        find_package(${QTX} REQUIRED)

        if (MSVC)
                link_libraries(optimized ${QTX} debug "${QTX}d")
        else (MSVC)
                link_libraries(${QTX})
        endif (MSVC)
endforeach()

