#include "diff.h"

namespace gitigor {

Diff::Diff(const QFileInfo& left, const QFileInfo& right)
    : left_(left), right_(right) {}

void Diff::startContext(size_t startLineLeft, size_t countLeft,
                        size_t startLineRight, size_t countRight,
                        const QString& context) {
    contexts_.push_back(DiffContext(startLineLeft, countLeft, startLineRight,
                                    countRight, left_.filePath(),
                                    right_.filePath(), context));
}

void Diff::pushLine(const QString& line, const QString& newLine) {
    if (contexts_.empty())
        startContext(0, 0, 0, 0, "");

    contexts_.back().push(DiffLine::Normal, line, newLine);
}

void Diff::pushNewLine(const QString& line, const QString& newLine) {
    if (contexts_.empty())
        startContext(0, 0, 0, 0, "");

    contexts_.back().push(DiffLine::Inserted, line, newLine);
}

void Diff::pushDeletedLine(const QString& line, const QString& newLine) {
    if (contexts_.empty())
        startContext(0, 0, 0, 0, "");

    contexts_.back().push(DiffLine::Deleted, line, newLine);
}

} // namespace gitigor
