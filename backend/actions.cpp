#include "actions.h"

#include <gkassert.h>

#include <QtWidgets/QAction>

namespace gitkit {
	
std::map<Actions::eAction, QAction*> Actions::actions_;

QAction* Actions::getAction(eAction id)
{
	auto it = actions_.find(id);
	if (it == actions_.end())
	{
		ASSERT(false);
		return nullptr;
	}
		
	return it->second;
}

bool Actions::initialise(QObject* parent)
{
	ASSERT(parent);
	if (!parent)
		return false;
		
	ASSERT(actions_.empty())
	if (!actions_.empty())
		return false;
	
	{
		auto action = new QAction(QIcon("data/icons/quit.png"), "&Quit", parent);
		action->setShortcuts(QKeySequence::Quit);
		action->setStatusTip("Quit the application");
		actions_[aFileQuit] = action;
	}
	
	{
		auto action = new QAction(QIcon("data/icons/sync.png"), "Refresh", parent);
		action->setShortcuts(QKeySequence::Refresh);
		action->setStatusTip("Refresh the UI");
		actions_[aFileRefresh] = action;
	}
	
	// aGitCommit
	
	{
		auto action = new QAction(QIcon("data/icons/fetch.png"), "Fetch", parent);
		action->setStatusTip("Git Fetch from default remote");
		actions_[aGitFetch] = action;
	}
	
	{
		auto action = new QAction(QIcon("data/icons/pull.png"), "Pull", parent);
		action->setStatusTip("Git Pull with default settings");
		actions_[aGitPull] = action;
	}
	
	{
		auto action = new QAction(QIcon("data/icons/push.png"), "Push", parent);
		action->setStatusTip("Git Push with default settings");
		actions_[aGitPush] = action;
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