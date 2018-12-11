#include "actions.h"

#include <QtCore/QDir>
#include <QtWidgets/QAction>

namespace gitigor
{

namespace
{
std::map<Actions::eAction, QAction*> s_actions;
}

QAction* Actions::getAction(eAction id)
{
    auto it = s_actions.find(id);
    if (it == s_actions.end()) {
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

    Q_ASSERT(s_actions.empty());
    if (!s_actions.empty())
        return false;

    QDir icons = dataPath;
    Q_ASSERT(icons.exists("res/icons"));
    if (!icons.cd("res/icons"))
        return false;

    {
        auto action = new QAction(QIcon(icons.filePath("quit.svg")), "&Quit", parent);
        action->setShortcuts(QKeySequence::Quit);
        action->setStatusTip("Quit the application");
        s_actions[aFileQuit] = action;
    }

    {
        auto action = new QAction(QIcon(icons.filePath("sync.svg")), "Refresh", parent);
        action->setShortcuts(QKeySequence::Refresh);
        action->setStatusTip("Refresh the UI");
        s_actions[aFileRefresh] = action;
    }

    // aGitCommit

    {
        auto action = new QAction(QIcon(icons.filePath("fetch.svg")), "Fetch", parent);
        action->setStatusTip("Git Fetch from default remote");
        s_actions[aGitFetch] = action;
    }

    {
        auto action = new QAction(QIcon(icons.filePath("pull.svg")), "Pull", parent);
        action->setStatusTip("Git Pull with default settings");
        s_actions[aGitPull] = action;
    }

    {
        auto action = new QAction(QIcon(icons.filePath("push.svg")), "Push", parent);
        action->setStatusTip("Git Push with default settings");
        s_actions[aGitPush] = action;
    }

    // Repositories view
    {
        auto action = new QAction(QIcon(icons.filePath("repo.svg")), "Edit", parent);
        action->setStatusTip("Edit repositories");
        s_actions[aRepoEdit] = action;
    }

    return true;
}

} // namespace gitigor
