set(module_name interface)

set(module_src
	includes.h
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

add_library(${module_name} SHARED ${module_src})
