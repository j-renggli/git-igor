#pragma once

#include <map>

class QAction;
class QDir;
class QObject;

namespace gitigor {

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
			aRepoEdit
		};
		
		static QAction* getAction(eAction id);
		
		static bool initialise(QObject* parent, const QDir& dataPath);
		
	private:
		//static QIcon loadSVG(const QString& path);
		
		static std::map<eAction, QAction*> actions_;
};

}