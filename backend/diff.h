#pragma once

#include <QtCore/QDir>

namespace gitkit {

class Diff
{
public:
    Diff(const QFileInfo& left, const QFileInfo& right);

    void addLine(const QString& line);

    const QStringList& lines() const { return lines_; }

private:
    QFileInfo left_;
    QFileInfo right_;

    QStringList lines_;
};

}
