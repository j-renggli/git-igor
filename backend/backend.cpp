#include "backend.h"

#include <iostream>

#include <QtWidgets/QAction>

#include "actions.h"
#include "repositorymanager.h"

#include "../gkassert.h"

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
	if (!RepositoryManager::instance().initialise())
		return false;
		
	const QString root(".config/gitkit");
	const QString css("css");
	
	configPath_ = QDir::home();
	ASSERT(configPath_.exists());
	configPath_.mkpath(root);
	bool ok = configPath_.cd(root);
	ASSERT(ok);
	
	configPath_.mkpath(css);
	cssPath_ = configPath_;
	ok = ok && cssPath_.cd(css);
	
	if (ok)
	{
		if (auto action = Actions::getAction(Actions::aFileRefresh))
			connect(action, SIGNAL(triggered()), this, SLOT(onRefresh()));
		if (auto action = Actions::getAction(Actions::aGitFetch))
			connect(action, SIGNAL(triggered()), this, SLOT(onFetch()));
		if (auto action = Actions::getAction(Actions::aGitPull))
			connect(action, SIGNAL(triggered()), this, SLOT(onPull()));
		if (auto action = Actions::getAction(Actions::aGitPush))
			connect(action, SIGNAL(triggered()), this, SLOT(onPush()));
	}
	
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
	auto& repos = RepositoryManager::instance();
	if (repos.empty())
		return;
		
	if (repos.active().fetch())
	{
		//emit onRepoUpdated();
	}
}

void Backend::onPull()
{
	auto& repos = RepositoryManager::instance();
	if (repos.empty())
		return;
		
	if (repos.active().pull())
	{
		//emit onRepoUpdated();
	}
}

void Backend::onPush()
{
	auto& repos = RepositoryManager::instance();
	if (repos.empty())
		return;
		
	if (repos.active().push())
	{
		//emit onRepoUpdated();
	}
}

}
