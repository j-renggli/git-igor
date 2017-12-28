#include "backend.h"

#include <QtWidgets/QAction>

#include "actions.h"
#include "repositorymanager.h"

namespace gitigor {

Backend Backend::s_backend;

Backend& Backend::instance()
{
	return s_backend;
}

Backend::Backend()
: resourcesPath_(SHARED_DATA_FOLDER)
{
}

const Repository& Backend::currentRepo() const
{
	return RepositoryManager::instance().active();
}

bool Backend::initialise(QObject* mainWin)
{
	if (!resourcesPath_.exists())
    {
        Q_ASSERT(false);
		return false;
	}
    const QString config(".config/git-igor");
	
	configPath_ = QDir::home();
    Q_ASSERT(configPath_.exists());
	configPath_.mkpath(config);
	bool ok = configPath_.cd(config);
    Q_ASSERT(ok);
	
	// Initialise actions
	ok = Actions::initialise(mainWin, resourcesPath_);
	if (!ok)
		return false;

	ok = runner_.initialise();
	if (!ok)
		return false;
		
	// Saved data
	QFileInfo repositories(configPath_, "repositories.json");
	if (!RepositoryManager::instance().initialise(repositories))
		return false;
		
	// HTML css
	const QString css("css");
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

void Backend::workStarted(const QString& command) { }//std::cout << command.toLatin1().data() << std::endl; }
void Backend::workEnded() { }//std::cout << "Work ended" << std::endl; }
void Backend::workOutput(const QString& output) { }//std::cout << output.toLatin1().data() << std::endl; }
	
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
	if (!proc.empty())
		runner_.enqueue(proc);
}

void Backend::onPull()
{
	auto& repos = RepositoryManager::instance();
	if (repos.empty())
		return;
		
    Process proc = repos.active().pull();
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

QFileInfo Backend::style() const
{
	// TODO: Restore which style from .config...
	QDir styles = resourcesPath_;
    Q_ASSERT(styles.exists("res/styles"));
	if (!styles.cd("res/styles"))
		return QFileInfo();
	return QFileInfo(styles.filePath("standard.qss"));
}

}
