#include "diff.h"

namespace gitkit {

Diff::Diff(const QFileInfo& left, const QFileInfo& right)
: left_(left)
, right_(right)
{
}

void Diff::addLine(const QString& line)
{
    lines_ << line;
}

}
