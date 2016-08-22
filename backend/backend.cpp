#include "backend.h"

#include <cassert>
#include <iostream>

#include <backend/repositorymanager.h>

namespace gitkit {

Backend Backend::s_backend;

Backend& Backend::instance()
{
	return s_backend;
}

Backend::Backend()
{
}

const Repository& Backend::currentRepo() const
{
	return RepositoryManager::instance().active();
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
	auto& repos = RepositoryManager::instance();
	if (repos.empty())
		return;
		
	if (repos.active().updateStatus())
	{
		emit onRepoUpdated();
	}
}

void Backend::onFetch()
{
}

}
