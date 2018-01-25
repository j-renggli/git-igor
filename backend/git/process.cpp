#include "process.h"

#include <QDataStream>
#include <QProcess>
#include <QtCore/QRegularExpression>

#include <iostream>
namespace gitigor {

const QRegularExpression GitProcess::s_rxLineEnd("[\r\n]");

#ifdef _WIN32
const QString git_cmd = "C:\\Program Files\\Git\\bin\\git.exe";
#else
const QString git_cmd = "git";
#endif

GitProcess::GitProcess(const QDir& root) : root_(root.absolutePath()) {
#ifdef _WIN32
    root_ = root_.mid(1).replace("/", "\\");
#endif
}

QStringList GitProcess::linesOut() const {
    return stdOut_.split(s_rxLineEnd, QString::SkipEmptyParts);
}

bool GitProcess::run(eCommand command, QStringList args, bool readOut,
                     bool readErr) {
    QProcess process;
    process.setWorkingDirectory(root_);

    process.start(git_cmd, prepareArgs(command, args));
    bool ok = process.waitForFinished();

    stdOut_.clear();
    stdErr_.clear();
    if (readOut)
        stdOut_ = process.readAllStandardOutput();
    if (readErr)
        stdErr_ = process.readAllStandardError();

    return ok;
}

bool GitProcess::runWithInput(eCommand command, QStringList args,
                              QByteArray input, bool readOut, bool readErr) {
    QProcess process;
    process.setWorkingDirectory(root_);

    process.start(git_cmd, prepareArgs(command, args));
    bool ok = process.waitForStarted();
    if (ok) {
        process.write(input);
        process.closeWriteChannel();
        ok = process.waitForFinished();
    }

    stdOut_.clear();
    stdErr_.clear();
    if (readOut)
        stdOut_ = process.readAllStandardOutput();
    if (readErr)
        stdErr_ = process.readAllStandardError();

    return ok;
}

QStringList GitProcess::prepareArgs(eCommand command, QStringList args) {
    switch (command) {
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
    case Log:
        args.insert(0, "log");
        args.insert(1, "--no-color");
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

} // namespace gitigor
