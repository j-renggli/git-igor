#include "logger.h"

#include <QtCore/QDebug>
#include <QtCore/QMetaType>
#include <QtCore/QProcess>
#include <QtCore/QRegularExpression>
#include <QtCore/QThread>

#include "process.h"

namespace {
const QRegularExpression rxCommitInfo(
    "^<(.*)><(.*)><(.*)><(.*)><(.*)><(.*)><(.*)><(.*)><(.*)><(.*)>:(.*)$");
}

namespace gitigor {

LogWorker::LogWorker(const QDir& root, const QString& head)
    : root_(root), head_(head) {}

void LogWorker::run() {
    QProcess process;
    process.setWorkingDirectory(root_.absolutePath());
    process.start("git", QStringList()
                             << "log"
                             << "--no-color"
                             << "--decorate=short"
                             << "--format=<%H><%P><%aN><%aE><%aI><%cN><%cE><%"
                                "cI><%D><%s>:%b" // %s %b === %B (?)
                             << head_,
                  QIODevice::ReadOnly);
    process.waitForStarted();
    while (!process.atEnd() || process.state() == QProcess::Running) {
        if (!process.waitForReadyRead())
            continue;

        while (process.canReadLine()) {
            onLineRead(QString(process.readLine()));
        }
    }

    // To be sure, send last item
    sendItem();
    /*
    while (true) {
        if (process.state() == QProcess::NotRunning) break;
        if (process.waitForReadyRead())
    while (process.state() != QProcess::NotRunning) {
        qDebug() << process.state();
        sleep(1);
    }
*/
    qDebug() << "Process finished";
}

void LogWorker::onLineRead(const QString& line) {
    auto commit = rxCommitInfo.match(line);
    if (commit.hasMatch()) {
        sendItem();
        Q_ASSERT(!current_);
        current_ = new LogItem();
        // "--format=<%H><%P><%aN><%aE><%aI><%cN><%cE><%cI><%D><%s>:%b"
        // <id><parents><author><email><date><commiter><email><date><branches/tags><summary>:full_message
        current_->id = commit.captured(1);
        current_->summary = commit.captured(10);
    } else {
        qDebug() << line;
    }
}

void LogWorker::sendItem() {
    if (current_) {
        emit onItem(*current_);
        current_ = nullptr;
    }
}

////////////////////////////////////////////////////////////////

GitLogger::GitLogger(const QDir& root) : root_(root) {}

GitLogger::~GitLogger() {
    if (isRunning()) {
        stop();
    }
}

bool GitLogger::isRunning() const {
    if (!workerThread_)
        return false;
    return workerThread_->isRunning();
}

bool GitLogger::start(const QString& head) {
    if (isRunning())
        return false;

    auto worker = new LogWorker(root_, head);
    connect(worker, &gitigor::LogWorker::onItem, this,
            &gitigor::GitLogger::onItem, Qt::QueuedConnection);
    workerThread_ = worker;
    workerThread_->start();

    onItem(LogItem());
    return true;
}

void GitLogger::stop() { Q_ASSERT(false); }

} // namespace gitigor
