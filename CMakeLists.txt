PROJECT(gitkit)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

SET(MAJOR_VERSION 0)
SET(MINOR_VERSION 1)
SET(PATCH_VERSION 0)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

SET(SHARED_DATA ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME})
SET(RES_PATH ${SHARED_DATA}/res)

INCLUDE(CPackConfig.cmake)

######## SET-UP ########

SET(QT_REQUIRED
Qt5Core
Qt5Gui
Qt5Widgets
Qt5WebEngineWidgets
)

SET(QT_DEPS
Qt5WebEngine
Qt5Network
Qt5MultimediaWidgets
Qt5PrintSupport
Qt5OpenGL
Qt5Multimedia
Qt5Positioning
Qt5Sensors
Qt5Qml
Qt5Quick
Qt5Sql
)

SET(QT_EXT
#icuin52
#icuuc52
#icudt52
)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

IF (MSVC)
        SET(QT_PATH "C:\\Qt\\5.8\\msvc2015_64\\")
	SET(CMAKE_PREFIX_PATH ${QT_PATH})
	LINK_DIRECTORIES("${QT_PATH}\\lib")
	SET(CMAKE_INCLUDE_CURRENT_DIR ON)
	SET(CMAKE_AUTOMOC ON)

	SET(QT_USED ${QT_REQUIRED} ${QT_DEPS})
	FOREACH(QTX ${QT_USED})
		MESSAGE(STATUS "Copying ${QT_PATH}/bin/${QTX}.dll TO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/%BUILD_TYPE%")
		FILE(COPY ${QT_PATH}/bin/${QTX}.dll
			DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
		)
		FILE(COPY ${QT_PATH}/bin/${QTX}d.dll
			DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
		)
	ENDFOREACH()
	
	FOREACH(QTX ${QT_EXT})
		MESSAGE(STATUS "Copying ${QT_PATH}/bin/${QTX}.dll TO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/%BUILD_TYPE%")
		FILE(COPY ${QT_PATH}/bin/${QTX}.dll
			DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
		)
		FILE(COPY ${QT_PATH}/bin/${QTX}.dll
			DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
		)
	ENDFOREACH()
ENDIF (MSVC)

######## DEPENDENCIES ########
FOREACH(QTX ${QT_REQUIRED})
	FIND_PACKAGE(${QTX} REQUIRED)
ENDFOREACH()

######## ALL DLLs ########

#ADD_SUBDIRECTORY(testing)

######## SETUP ########

STRING(REPLACE "\\" "\\\\" SHARED_DATA_ESCAPED ${SHARED_DATA})

SET(CMAKE_VERBOSE_MAKEFILE 0)
ADD_COMPILE_OPTIONS(-std=c++11)
SET(CMAKE_CXX_FLAGS "-Wall -ansi -pedantic -g -pg -fpic -fvisibility=hidden -fvisibility-inlines-hidden")
IF(MSVC)
        SET(FLAGS_COMMON "/Zi /Gm-")  # /WX
	SET(FLAGS_DEBUG "/Od /Ob0")
	SET(FLAGS_RELEASE "/Ox /Oi /Ot /Oy /GL /GS- /Gy /D _SECURE_SCL=0 ")
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${FLAGS_COMMON} ${FLAGS_DEBUG} -O -DDEBUG -DSHARED_DATA_FOLDER=\"\\\"${PROJECT_SOURCE_DIR}/data\\\"\"")
        SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${FLAGS_COMMON} ${FLAGS_RELEASE} /Zi -O3 -DSHARED_DATA_FOLDER=\"\\\"${SHARED_DATA_ESCAPED}\\\"\"")
ELSE(MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wno-long-long")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -DDEBUG -DSHARED_DATA_FOLDER=\"\\\"${PROJECT_SOURCE_DIR}/data\\\"\"")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -O3 -DSHARED_DATA_FOLDER=\"\\\"${SHARED_DATA_ESCAPED}\\\"\"")
ENDIF(MSVC)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)

INCLUDE_DIRECTORIES(. ${Qt5Core_INCLUDE_DIRS})

######## QT ########

SET(CMAKE_AUTOMOC ON)

######## SOURCE CODE ########

FILE(GLOB GITKIT_H *.h)

SET(GITKIT_SRC
#commandline.cpp
gitkit.cpp
mainwin.cpp

backend/actions.cpp
backend/backend.cpp
backend/repository.cpp
backend/repositorymanager.cpp
backend/runaction.cpp
backend/runner.cpp

interface/progress.cpp
interface/repo_add.cpp
interface/repositories.cpp
interface/staging.cpp
interface/view.cpp

preferences/preferences.cpp
)

######## LINKER ########

IF (MSVC)
  LINK_LIBRARIES(
    optimized Qt5Core debug Qt5Cored
    optimized Qt5Gui debug Qt5Guid
    optimized Qt5Widgets debug Qt5Widgetsd
    optimized Qt5WebEngineWidgets debug Qt5WebEngineWidgetsd
    optimized Qt5WebEngine debug Qt5WebEngined
  )
ELSE (MSVC)
  LINK_LIBRARIES(
    #boost_filesystem
    boost_system

    #Qt5Quick
    Qt5Core
    Qt5Gui
    #Qt5Qml
    Qt5Widgets
    Qt5WebEngineWidgets
    Qt5WebEngine
  )
ENDIF (MSVC)

######## EXECUTABLE ########

ADD_EXECUTABLE(${PROJECT_NAME} ${GITKIT_SRC})

IF (MSVC)
	#GET_TARGET_PROPERTY(QtCore_location Qt5::Core LOCATION)
	#TARGET_LINK_LIBRARIES(${PROJECT_NAME} QtCore_LIBRARIES)
ENDIF (MSVC)

######## INSTALLATION ########

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
INSTALL(DIRECTORY data/res/icons DESTINATION ${RES_PATH})
INSTALL(DIRECTORY data/res/styles DESTINATION ${RES_PATH})
#SET(BIN_INSTALL_DIR "bin")

