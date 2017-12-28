#include "diff_io.h"

#include <QtWebEngineWidgets/QWebEnginePage>

#include <backend/backend.h>

namespace gitigor {

UIDiffIO::UIDiffIO(QObject* parent, QWebEnginePage* page)
    : QObject(parent)
    , page_(page)
{
    Q_ASSERT(page_);
}

void UIDiffIO::setActiveDiff(const std::vector<Diff>& diff)
{
    currentDiff_.clear();
    for (const auto& d : diff)
        currentDiff_.push_back(d);
}

void UIDiffIO::onStageHunk(int hunkIndex)
{
    auto contexts = currentDiff_.at(0).contexts();
    auto fileInfo = currentDiff_.at(0).fileInfo();

    const size_t index(hunkIndex);
    if (index > contexts.size())
    {
        Q_ASSERT(false); // TODO: More than just assert...
        return;
    }

    const auto& hunk = contexts.at(index);

    Backend& backend = Backend::instance();
    const Repository& repo = backend.currentRepo();
    repo.stage(hunk);
    backend.onRefresh();
}

}
