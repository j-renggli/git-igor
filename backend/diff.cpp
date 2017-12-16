#include "diff.h"

namespace gitkit {

Diff::Diff(const QFileInfo& left, const QFileInfo& right)
: left_(left)
, right_(right)
{
}

void Diff::startContext(size_t startLineLeft, size_t countLeft, size_t startLineRight, size_t countRight, const QString& context)
{
    contexts_.push_back(DiffContext(startLineLeft, countLeft, startLineRight, countRight, context));
}

void Diff::pushLine(const QString& line)
{
    if (contexts_.empty())
        startContext(0, 0, 0, 0, "");

    contexts_.back().push(DiffLine::Normal, line);
}

void Diff::pushNewLine(const QString& line)
{
    if (contexts_.empty())
        startContext(0, 0, 0, 0, "");

    contexts_.back().push(DiffLine::Inserted, line);
}

void Diff::pushDeletedLine(const QString& line)
{
    if (contexts_.empty())
        startContext(0, 0, 0, 0, "");

    contexts_.back().push(DiffLine::Deleted, line);
}

////////////////////////////////////////////////////////////////

void DiffContext::setNoNewline(bool sideNew)
{
    if (sideNew)
        noNewlineNew_ = true;
    else
        noNewlineOld_ = true;
}

}
