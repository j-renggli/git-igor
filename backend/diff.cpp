#include "diff.h"

namespace gitkit {

Diff::Diff(const QFileInfo& left, const QFileInfo& right)
: left_(left)
, right_(right)
{
}

void Diff::startContext(size_t startLineLeft, size_t countLeft, size_t startLineRight, size_t countRight, const QString& context)
{
    contexts_.push_back(DiffContext(startLineLeft, countLeft, startLineRight, countRight, left_.filePath(), right_.filePath(), context));
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

QString DiffContext::toPatch() const
{
    QString patch = QString("diff --git a/%1 b/%2\n"
                            "--- a/%1\n"
                            "+++ b/%2\n"
                            "@@ -%3,%4 +%5,%6 @@\n")
            .arg(fileOld_)
            .arg(fileNew_)
            .arg(startOld_)
            .arg(countOld_)
            .arg(startNew_)
            .arg(countNew_);

    for (const auto& line : lines_)
    {
        QString type;
        switch (line.type())
        {
        case DiffLine::Normal:
            type = " ";
            break;
        case DiffLine::Inserted:
            type = "+";
            break;
        case DiffLine::Deleted:
            type = "-";
            break;
        }
        patch += QString("%1%2\n").arg(type).arg(line.text());
    }
    return patch;
}

}
