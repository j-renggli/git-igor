set(target interface)

set(target_src
	progress.cpp
	progress.h
	repo_add.cpp
	repo_add.h
	repositories.cpp
	repositories.h

	diff/io.cpp
	diff/io.h
	diff/view.cpp
	diff/view.h

	history/io.cpp
	history/io.h
	history/view.cpp
	history/view.h

	staging/view.cpp
	staging/view.h

	../preferences/preferences.cpp
	../preferences/preferences.h
	)

add_library(${target} SHARED ${target_src})

include(GenerateExportHeader)

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${target} backend)

generate_export_header(${target}
    BASE_NAME ${target} # name for add_definitions
    EXPORT_MACRO_NAME ${target}_EXPORT # macro name
    EXPORT_FILE_NAME ${target}_Export.h # file name
)
