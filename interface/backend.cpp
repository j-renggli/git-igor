#include "backend.h"

#include <cassert>
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

bool Backend::initialise()
{
	const QString root(".config/gitkit");
	const QString css("css");
	
	configPath_ = QDir::home();
	assert(configPath_.exists());
	configPath_.mkpath(root);
	bool ok = configPath_.cd(root);
	assert(ok);
	
	configPath_.mkpath(css);
	cssPath_ = configPath_;
	ok = ok && cssPath_.cd(css);
	
	return ok;
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
