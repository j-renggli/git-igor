SET(QT_REQUIRED
Qt5Core
Qt5Gui
Qt5Widgets
Qt5WebEngine
Qt5WebEngineWidgets
)

SET(QT_DEPS
)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET(CMAKE_AUTOMOC ON)

IF (MSVC)
	SET(QT_PATH "C:\\Qt\\5.8\\msvc2015_64\\")
	SET(CMAKE_PREFIX_PATH ${QT_PATH})
	LINK_DIRECTORIES("${QT_PATH}\\lib")
	SET(CMAKE_INCLUDE_CURRENT_DIR ON)

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

FOREACH(QTX ${QT_REQUIRED})
	FIND_PACKAGE(${QTX} REQUIRED)

	IF (MSVC)
		LINK_LIBRARIES(optimized ${QTX} debug "${QTX}d")
	ELSE (MSVC)
		LINK_LIBRARIES(${QTX})
	ENDIF (MSVC)
ENDFOREACH()

