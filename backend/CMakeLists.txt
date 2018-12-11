set(target backend)

set(target_src
	actions.cpp
	actions.h
	backend.cpp
	backend.h
	repository.cpp
	repository.h
	repositorymanager.cpp
	repositorymanager.h
	runaction.cpp
	runaction.h
	runner.cpp
	runner.h

	diff/context.cpp
	diff/context.h
	diff/diff.cpp
	diff/diff.h
	diff/line.cpp
	diff/line.h

	git/logger.cpp
	git/logger.h
	git/process.cpp
	git/process.h
	)

add_library(${target} SHARED ${target_src})

include(GenerateExportHeader)

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(${target}
    BASE_NAME ${target} # name for add_definitions
    EXPORT_MACRO_NAME ${target}_EXPORT # macro name
    EXPORT_FILE_NAME ${target}_Export.h # file name
)

