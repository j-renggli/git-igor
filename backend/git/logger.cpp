#include "logger.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QtCore/QDebug>
#include <QtCore/QMetaType>
#include <QtCore/QProcess>
#include <QtCore/QRegularExpression>
#include <QtCore/QThread>

#include "process.h"

namespace {
const QRegularExpression rxCommitInfo("^<([a-f0-9]{40})><((?: "
                                      "?[a-f0-9]{40})+)><(.*)><(.+@.+)><(.*)><("
                                      ".*)><(.+@.+)><(.*)><(.*)><(.*)>:(.*)$");

#ifdef _WIN32
const QString git_cmd = "C:\\Program Files\\Git\\bin\\git.exe";
#else
const QString git_cmd = "git";
#endif

}

namespace gitigor {

LogWorker::LogWorker(const QDir& root, const QString& head)
    : root_(root), head_(head) {}

void LogWorker::run() {
    QProcess process;
    process.setWorkingDirectory(root_.absolutePath().mid(1).replace("/", "\\"));
    process.start(git_cmd, QStringList()
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
        current_ = new QJsonObject();
        QJsonObject& item = *current_;
        item["id"] = commit.captured(1);
        QJsonArray parents;
        for (const auto& parent : commit.captured(2).split(" "))
            parents.append(parent);
        item["parents"] = parents;
        item["summary"] = commit.captured(10);

        // "--format=<%H><%P><%aN><%aE><%aI><%cN><%cE><%cI><%D><%s>:%b"
        // <id><parents><author><email><date><commiter><email><date><branches/tags><summary>:full_message
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

    return true;
}

void GitLogger::stop() { Q_ASSERT(false); }

} // namespace gitigor
