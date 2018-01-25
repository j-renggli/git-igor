set(module_name backend)

set(module_src
	actions.cpp
	actions.h
	backend.cpp
	backend.h
	includes.h
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

add_definitions(-DDLL_BACKEND)

add_library(${module_name} SHARED ${module_src})
