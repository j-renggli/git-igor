#pragma once

#include <map>

#include "includes.h"

class QAction;
class QDir;
class QObject;

namespace gitigor {

class backend_EXPORT Actions {
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
    static std::map<eAction, QAction*> actions_;
};
}
