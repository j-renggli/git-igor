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

void DiffContext::push(DiffLine::LineType type, const QString& text)
{
    size_t line = 0;
    size_t lineAlt(-1);
    switch(type)
    {
    case DiffLine::Normal:
        line = startOld_ + countOld_++;
        lineAlt = startNew_ + countNew_++;
        break;
    case DiffLine::Inserted:
        line = startNew_ + countNew_++;
        break;
    case DiffLine::Deleted:
        line = startOld_ + countOld_++;
        break;
    }

    lines_.push_back(DiffLine(type, text, line, lineAlt));
}

void DiffContext::setNoNewline(bool sideNew)
{
    if (sideNew)
        noNewlineNew_ = true;
    else
        noNewlineOld_ = true;
}

}
