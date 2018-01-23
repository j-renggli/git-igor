#include "io.h"

#include <QtWebEngineWidgets/QWebEnginePage>

#include <backend/backend.h>

namespace gitigor {

DiffIO::DiffIO(QObject* parent, QWebEnginePage* page)
    : QObject(parent), page_(page) {
    Q_ASSERT(page_);
}

DiffIO::~DiffIO() {
    if (currentDiff_)
        delete currentDiff_;
}

void DiffIO::setActiveDiff(const Diff& diff) {
    if (currentDiff_)
        delete currentDiff_;
    currentDiff_ = new Diff(diff);
}

void DiffIO::onStageHunk(int hunkIndex) {
    if (!currentDiff_)
        return;

    auto contexts = currentDiff_->contexts();
    auto fileInfo = currentDiff_->fileInfo();

    const size_t index(hunkIndex);
    if (index > contexts.size()) {
        Q_ASSERT(false); // TODO: More than just assert...
        return;
    }

    const auto& hunk = contexts.at(index);

    Backend& backend = Backend::instance();
    const Repository& repo = backend.currentRepo();
    repo.stage(hunk);
    backend.onRefresh();
}

bool DiffIO::onStageLines(int hunkIndex, int begin, int end) {
    // TODO: return update !?
    qDebug() << "Staging lines [" << hunkIndex << ", " << begin << " -> " << end
             << "]";

    if (!currentDiff_)
        return false;

    auto contexts = currentDiff_->contexts();
    auto fileInfo = currentDiff_->fileInfo();

    const size_t index(hunkIndex);
    if (index > contexts.size()) {
        Q_ASSERT(false); // TODO: More than just assert...
        return false;
    }

    const auto& hunk = contexts.at(index);
    qDebug("%s\n--------------------------------", qPrintable(hunk.toPatch()));

    auto selected = hunk.selectLines(begin, end);
    qDebug("%s\n--------------------------------",
           qPrintable(selected.toPatch()));

    Backend& backend = Backend::instance();

    const Repository& repo = backend.currentRepo();
    repo.stage(selected);

    backend.onRefresh(true);
    return false;
}

} // namespace gitigor
