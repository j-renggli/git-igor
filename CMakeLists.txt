PROJECT(gitkit)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

#FIND_PACKAGE(Qt5Quick REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5WebKitWidgets REQUIRED)

######## ALL DLLs ########

#ADD_SUBDIRECTORY(testing)

######## SETUP ########

SET(CMAKE_VERBOSE_MAKEFILE 0)
SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -Werror -O -ansi -pedantic -std=c++11 -g -pg -fpic -fvisibility=hidden -fvisibility-inlines-hidden -Wno-long-long -DDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -Werror -O3 -ansi -pedantic -std=c++11 -g -pg -fpic -fvisibility=hidden -fvisibility-inlines-hidden -Wno-long-long")

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

ADD_EXECUTABLE(gitkit ${GITKIT_SRC})

