#include "actions.h"

#include <QtCore/QDir>
#include <QtWidgets/QAction>

namespace gitigor {
	
std::map<Actions::eAction, QAction*> Actions::actions_;

QAction* Actions::getAction(eAction id)
{
	auto it = actions_.find(id);
	if (it == actions_.end())
	{
        Q_ASSERT(false);
		return nullptr;
	}
		
	return it->second;
}

bool Actions::initialise(QObject* parent, const QDir& dataPath)
{
    Q_ASSERT(parent);
	if (!parent)
		return false;
		
    Q_ASSERT(actions_.empty());
	if (!actions_.empty())
		return false;
	
	QDir icons = dataPath;
    Q_ASSERT(icons.exists("res/icons"));
	if (!icons.cd("res/icons"))
		return false;
	
	{
        auto action = new QAction(QIcon(icons.filePath("quit.svg")), "&Quit", parent);
		action->setShortcuts(QKeySequence::Quit);
		action->setStatusTip("Quit the application");
		actions_[aFileQuit] = action;
	}
	
	{
        auto action = new QAction(QIcon(icons.filePath("sync.svg")), "Refresh", parent);
		action->setShortcuts(QKeySequence::Refresh);
		action->setStatusTip("Refresh the UI");
		actions_[aFileRefresh] = action;
	}
	
	// aGitCommit
	
	{
        auto action = new QAction(QIcon(icons.filePath("fetch.svg")), "Fetch", parent);
		action->setStatusTip("Git Fetch from default remote");
		actions_[aGitFetch] = action;
	}
	
	{
        auto action = new QAction(QIcon(icons.filePath("pull.svg")), "Pull", parent);
		action->setStatusTip("Git Pull with default settings");
		actions_[aGitPull] = action;
	}
	
	{
        auto action = new QAction(QIcon(icons.filePath("push.svg")), "Push", parent);
		action->setStatusTip("Git Push with default settings");
		actions_[aGitPush] = action;
	}
	
	// Repositories view
	{
        auto action = new QAction(QIcon(icons.filePath("repo.svg")), "Edit", parent);
		action->setStatusTip("Edit repositories");
		actions_[aRepoEdit] = action;
	}

    return true;
}
/*
QIcon Actions::loadSVG(const QString& path)
{
	QPixmap pixmap(64, 64);
	QPainter painter(&pixmap);
	painter.setBackgroundMode(Qt::OpaqueMode);
	QSvgRenderer renderer(path);
	renderer.render(&painter);
	return QIcon(pixmap);
}
*/
}
