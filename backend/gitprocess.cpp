#include "gitprocess.h"

#include <QProcess>

namespace gitkit {

const QRegExp GitProcess::s_rxLineEnd("[\r\n]");

GitProcess::GitProcess(const QDir& root)
: root_(root.absolutePath())
{
}

QStringList GitProcess::linesOut() const
{
    return stdOut_.split(s_rxLineEnd, QString::SkipEmptyParts);
}

bool GitProcess::run(eCommand command, QStringList args, bool readOut, bool readErr)
{
    QProcess process;
    process.setWorkingDirectory(root_);
    switch (command)
    {
    case Commit:
        args.insert(0, "commit");
        break;
    case Diff:
        args.insert(0, "diff");
        break;
    case RevParse:
        args.insert(0, "rev-parse");
        break;
    case Show:
        args.insert(0, "show");
        break;
    case Status:
        args.insert(0, "status");
        break;
    }

    process.start("git", args);
    bool ok = process.waitForFinished();

    if (ok)
    {
        if (readOut)
            stdOut_ = process.readAllStandardOutput();
        else
            stdOut_.clear();
    }

    return ok;
}

}

