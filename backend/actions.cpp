#include "actions.h"

#include <gkassert.h>

#include <QtWidgets/QAction>

namespace gitkit {
	
std::map<Actions::eAction, QAction*> Actions::actions_;

QAction* Actions::getAction(eAction id)
{
	auto it = actions_.find(id);
	if (it == actions_.end())
		return nullptr;
		
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
		auto action = new QAction("&Quit", parent);
		action->setShortcuts(QKeySequence::Quit);
		action->setStatusTip("Quit the application");
		actions_[aFileQuit] = action;
	}
	
	{
		auto action = new QAction("Refresh", parent);
		action->setShortcuts(QKeySequence::Refresh);
		action->setStatusTip("Refresh the UI");
		actions_[aFileRefresh] = action;
	}

  return true;
}

}