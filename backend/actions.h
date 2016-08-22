#pragma once

#include <map>

class QAction;
class QObject;

namespace gitkit {

class Actions
{
	public:
		enum eAction {
			aFileQuit,
			aFileRefresh,
			aGitCommit,
			aGitFetch,
			aGitPull,
			aGitPush,
		};
		
		static QAction* getAction(eAction id);
		
		static bool initialise(QObject* parent);
		
	private:
		static std::map<eAction, QAction*> actions_;
};

}