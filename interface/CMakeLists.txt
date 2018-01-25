set(module_name interface)

set(module_src
	history_io.cpp
	history_io.h
	history_view.cpp
	history_view.h
	includes.h
	progress.cpp
	progress.h
	repo_add.cpp
	repo_add.h
	repositories.cpp
	repositories.h
	staging.cpp
	staging.h

	diff/io.cpp
	diff/io.h
	diff/view.cpp
	diff/view.h

	../preferences/preferences.cpp
	../preferences/preferences.h
	)

add_library(${module_name} SHARED ${module_src})
