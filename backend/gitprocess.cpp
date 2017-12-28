#include "gitprocess.h"

#include <QProcess>
#include <QDataStream>
#include <QtCore/QRegularExpression>

namespace gitigor {

const QRegularExpression GitProcess::s_rxLineEnd("[\r\n]");

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

    process.start("git", prepareArgs(command, args));
    bool ok = process.waitForFinished();

    stdOut_.clear();
    stdErr_.clear();
    if (ok)
    {
        if (readOut)
            stdOut_ = process.readAllStandardOutput();
    }
    else
    {
        if (readErr)
            stdErr_ = process.readAllStandardError();
    }

    return ok;
}

bool GitProcess::runWithInput(eCommand command, QStringList args, QByteArray input, bool readOut, bool readErr)
{
    QProcess process;
    process.setWorkingDirectory(root_);

    process.start("git", prepareArgs(command, args));
    bool ok = process.waitForStarted();
    if (ok)
    {
        process.write(input);
        process.closeWriteChannel();
        ok = process.waitForFinished();
    }

    stdOut_.clear();
    stdErr_.clear();
    if (ok)
    {
        if (readOut)
            stdOut_ = process.readAllStandardOutput();
    }
    else
    {
        if (readErr)
            stdErr_ = process.readAllStandardError();
    }

    return ok;
}

QStringList GitProcess::prepareArgs(eCommand command, QStringList args)
{
    switch (command)
    {
    case Add:
        args.insert(0, "add");
        break;
    case Apply:
        args.insert(0, "apply");
        break;
    case Commit:
        args.insert(0, "commit");
        break;
    case Delete:
        args.insert(0, "rm");
        break;
    case Diff:
        args.insert(0, "diff");
        args.insert(1, "--no-color");
        break;
    case Push:
        args.insert(0, "push");
        break;
    case Reset:
        args.insert(0, "reset");
        break;
    case RevParse:
        args.insert(0, "rev-parse");
        break;
    case Show:
        args.insert(0, "show");
        break;
    case Status:
        args.insert(0, "status");
        args.insert(1, "--porcelain");
        break;
    }

    return args;
}

}

