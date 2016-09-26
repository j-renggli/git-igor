PROJECT(gitkit)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

SET(MAJOR_VERSION 0)
SET(MINOR_VERSION 1)
SET(PATCH_VERSION 0)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

SET(SHARED_DATA ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME})
SET(RES_PATH ${SHARED_DATA}/res)

INCLUDE(CPackConfig.cmake)

######## DEPENDENCIES ########

#FIND_PACKAGE(Qt5Quick REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5WebKitWidgets REQUIRED)

######## ALL DLLs ########

#ADD_SUBDIRECTORY(testing)

######## SETUP ########

STRING(REPLACE "\\" "\\\\" SHARED_DATA_ESCAPED ${SHARED_DATA})

SET(CMAKE_VERBOSE_MAKEFILE 0)
SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -Werror -O -ansi -pedantic -std=c++11 -g -pg -fpic -fvisibility=hidden -fvisibility-inlines-hidden -Wno-long-long -DDEBUG -DSHARED_DATA_FOLDER=\"\\\"${PROJECT_SOURCE_DIR}/data\\\"\"")
SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -Werror -O3 -ansi -pedantic -std=c++11 -g -pg -fpic -fvisibility=hidden -fvisibility-inlines-hidden -Wno-long-long -DSHARED_DATA_FOLDER=\"\\\"${SHARED_DATA_ESCAPED}\\\"\"")

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

LINK_LIBRARIES(
#boost_filesystem
boost_system

#Qt5Quick
Qt5Core
Qt5Gui
#Qt5Qml
Qt5Widgets
Qt5WebKitWidgets
Qt5WebKit
)

######## EXECUTABLE ########

ADD_EXECUTABLE(${PROJECT_NAME} ${GITKIT_SRC})

######## INSTALLATION ########

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
INSTALL(DIRECTORY data/res/icons DESTINATION ${RES_PATH})
#SET(BIN_INSTALL_DIR "bin")

