project(git-igor)

cmake_minimum_required(VERSION 3.0)

set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)
set(PATCH_VERSION 0)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(SHARED_DATA ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME})
set(RES_PATH ${SHARED_DATA}/res)

include(CPackConfig.cmake)

######## QT ########

include(Qt)

include_directories(. ${Qt5Core_INCLUDE_DIRS})

######## DEPENDENCIES ########

######## SETUP ########

STRING(REPLACE "\\" "\\\\" SHARED_DATA_ESCAPED ${SHARED_DATA})

SET(CMAKE_VERBOSE_MAKEFILE 0)
ADD_COMPILE_OPTIONS(-std=c++17)
SET(CMAKE_CXX_FLAGS "-Wall -ansi -pedantic -g -pg -fpic -fvisibility=hidden -fvisibility-inlines-hidden")
IF(MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /Gm-")  # /WX
	SET(FLAGS_DEBUG "/Od /Ob0")
	SET(FLAGS_RELEASE "/Ox /Oi /Ot /Oy /GL /GS- /Gy /D _SECURE_SCL=0")
		SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${FLAGS_DEBUG} -DSHARED_DATA_FOLDER=\"\\\"${PROJECT_SOURCE_DIR}/data\\\"\"")
		SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${FLAGS_RELEASE} -DSHARED_DATA_FOLDER=\"\\\"${SHARED_DATA_ESCAPED}\\\"\"")
ELSE(MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wno-long-long")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -DDEBUG -DSHARED_DATA_FOLDER=\"\\\"${PROJECT_SOURCE_DIR}/data\\\"\"")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -O3 -DSHARED_DATA_FOLDER=\"\\\"${SHARED_DATA_ESCAPED}\\\"\"")
ENDIF(MSVC)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

######## ALL DLLs ########

add_subdirectory(backend)
add_subdirectory(interface)

######## SOURCE CODE ########

file(GLOB GITIGOR_H *.h)
file(GLOB GITIGOR_PREFS preferences/*.cpp)

set(GITIGOR_SRC
#commandline.cpp
git-igor.cpp
mainwin.cpp

${GITIGOR_PREFS}

${GITIGOR_H}
)

######## LINKER ########

######## EXECUTABLE ########

add_executable(${PROJECT_NAME} ${GITIGOR_SRC})

target_link_libraries(${PROJECT_NAME} backend interface)

IF (MSVC)
	#GET_TARGET_PROPERTY(QtCore_location Qt5::Core LOCATION)
	#TARGET_LINK_LIBRARIES(${PROJECT_NAME} QtCore_LIBRARIES)
ENDIF (MSVC)

######## INSTALLATION ########

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(DIRECTORY data/res/icons DESTINATION ${RES_PATH})
install(DIRECTORY data/res/styles DESTINATION ${RES_PATH})
#SET(BIN_INSTALL_DIR "bin")

