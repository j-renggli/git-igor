PROJECT(ccc)

cmake_minimum_required(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

FIND_PACKAGE(Qt4 REQUIRED)

######## ALL DLLs ########

#ADD_SUBDIRECTORY(testing)

######## SETUP ########

SET(CMAKE_VERBOSE_MAKEFILE 0)
SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -O -ansi -pedantic -std=c++11 -g -pg -Wno-long-long -DDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -ansi -pedantic -std=c++11 -g -pg -Wno-long-long")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)

INCLUDE_DIRECTORIES(. /usr/include/qt4)

######## QT ########

# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
#   set(QT_USE_OPENGL TRUE)
# -> this will cause cmake to include and link against the OpenGL module
#SET(QT_USE_OPENGL TRUE)
include(${QT_USE_FILE})

# the variable "QT_SRC" contains all .cpp files of this project
SET(QT_SRC
commandline.cpp
mainwin.cpp
)

SET(QT_H
commandline.h
mainwin.h
)

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
SET(QT_WRAP_UI "1")

QT4_WRAP_UI(QT_UI_H ${QT_UI})
QT4_WRAP_CPP(QT_MOC_SRC ${QT_H})

######## SOURCE CODE ########

SET(CCC_SRC
ccc.cpp
)

######## LINKER ########

LINK_LIBRARIES(
boost_filesystem
boost_system

QtGui
)

######## EXECUTABLE ########

ADD_EXECUTABLE(ccc ${CCC_SRC} ${QT_SRC} ${QT_MOC_SRC} ${QT_UI_H})
