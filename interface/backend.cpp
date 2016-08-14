#include "backend.h"

#include <iostream>

#include <backend/repository.h>

namespace gitkit {

Backend Backend::s_backend;

Backend& Backend::instance()
{
	return s_backend;
}

Backend::Backend()
: current_repo_(0)
{
	repositories_.push_back(Repository());
}
	
void Backend::onRefresh()
{
	if (repositories_.at(current_repo_).updateStatus())
		emit onRepoUpdated();
}

void Backend::onFetch()
{
}

}
