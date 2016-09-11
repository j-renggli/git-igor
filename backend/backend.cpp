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
			
		connect(&runner_, &ActionRunner::notifyStartWork, this, &Backend::workStarted);
		connect(&runner_, &ActionRunner::notifyStopWork, this, &Backend::workEnded);
		connect(&runner_, &ActionRunner::notifyOutput, this, &Backend::workOutput);
	}
	
	return ok;
}

void Backend::workStarted(const QString& command) { std::cout << command.toLatin1().data() << std::endl; }
void Backend::workEnded() { std::cout << "Work ended" << std::endl; }
void Backend::workOutput(const QString& output) { std::cout << output.toLatin1().data() << std::endl; }
	
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
		
	Process proc = repos.active().fetch();
	std::cout << proc.empty() << std::endl;
	if (!proc.empty())
		runner_.enqueue(proc);
}

void Backend::onPull()
{
	auto& repos = RepositoryManager::instance();
	if (repos.empty())
		return;
		
	Process proc = repos.active().pull();
	std::cout << proc.empty() << std::endl;
	if (!proc.empty())
		runner_.enqueue(proc);
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
