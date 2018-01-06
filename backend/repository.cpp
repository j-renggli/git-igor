#include "repository.h"

#include <sstream>
#include <stack>

#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

#include "runaction.h"

#include "git/logger.h"
#include "git/process.h"

namespace gitigor {

const QRegularExpression
    Repository::s_rxDiffFiles("^diff --git a[/](.+) b[/](.+)$");
const QRegularExpression Repository::s_rxDiffContext(
    "^@@ [-](\\d+),(\\d+) [+](\\d+),(\\d+) @@(?: (.+))?$");

FileStatus::FileStatus(const QFileInfo &path, eStatus index, eStatus workTree,
                       bool conflict)
    : path_(path), index_(index), workTree_(workTree), conflict_(conflict) {}

bool FileStatus::operator<(const FileStatus &rhs) const {
    return path_.filePath() < rhs.path_.filePath();
}

////////////////////////////////////////////////////////////////

Repository::Repository(const QString &name, const QDir &root)
    : root_(root), name_(name) {}

Repository::Repository(const Repository &copy) { Q_ASSERT(false); }

bool Repository::commit(const QString &message) const {
    if (message.isEmpty())
        return false;

    GitProcess process(root_);
    return process.run(GitProcess::Commit, QStringList() << "-m" << message,
                       false, false);
}

std::vector<Diff> Repository::diff(const FileStatus &file, bool indexed) const {
    std::vector<Diff> diffs;

    auto parseText = [](const QString &text,
                        std::function<void(const QString &, const QString &,
                                           const QString &, const QString &)>
                            onLine) {
        static QRegularExpression rxLine(
            "((.)(.*))?(\\r\\n|\\n)"); //, QRegularExpression::MultilineOption);
        static const int groupLine = 1;
        static const int groupPrefix = 2;
        static const int groupNoPrefix = 3;
        static const int groupNewLine = 4;

        auto rx_it = rxLine.globalMatch(text);

        while (rx_it.hasNext()) {
            auto lineMatch = rx_it.next();
            const QString line = lineMatch.captured(groupLine);
            const QString prefix = lineMatch.captured(groupPrefix);
            const QString noPrefix = lineMatch.captured(groupNoPrefix);
            const QString newLine = lineMatch.captured(groupNewLine);

            onLine(line, prefix, noPrefix, newLine);
        }
    };

    // New file ? Whole file added !
    FileStatus::eStatus workTreeStatus = file.status(false);
    if (workTreeStatus == FileStatus::ADDED) {
        QFile disk_file(root_.filePath(file.path().filePath()));
        if (disk_file.open(QIODevice::ReadOnly)) {
            QTextStream in(&disk_file);
            diffs.push_back(Diff(QFileInfo(), file.path()));
            parseText(in.readAll(),
                      [&](const QString &line, const QString &prefix,
                          const QString &noPrefix, const QString &newLine) {
                          diffs.back().pushNewLine(line, newLine);
                      });
            disk_file.close();
        }

        return diffs;
    } else if (workTreeStatus == FileStatus::DELETED ||
               file.status(true) ==
                   FileStatus::DELETED) // 2nd for staged deleted files...
    {
        diffs.push_back(Diff(file.path(), file.path()));
        QString head("HEAD:");
        head += file.path().filePath();

        GitProcess process(root_);
        process.run(GitProcess::Show, QStringList() << head, true, false);
        parseText(process.out(),
                  [&](const QString &line, const QString &prefix,
                      const QString &noPrefix, const QString &newLine) {
                      diffs.back().pushDeletedLine(line, newLine);
                  });
        return diffs;
    }

    QStringList args;
    if (indexed)
        args << "--cached";
    args << file.path().filePath();

    GitProcess process(root_);
    process.run(GitProcess::Diff, args, true, false);

    QFileInfo left, right;
    QString context;
    bool hasContext = false;

    enum Previous { ADD, DEL, BOTH };
    Previous lastOp = BOTH;

    parseText(process.out(),
              [&](const QString &line, const QString &prefix,
                  const QString &noPrefix, const QString &newLine) {
                  auto fileMatch = s_rxDiffFiles.match(line);
                  if (fileMatch.hasMatch()) {
                      left = fileMatch.captured(1);
                      right = fileMatch.captured(2);
                      context = "";
                      hasContext = false;
                      diffs.push_back(Diff(left, right));
                      return;
                  }

                  auto contextMatch = s_rxDiffContext.match(line);
                  if (contextMatch.hasMatch()) {
                      int startDel = contextMatch.captured(1).toInt();
                      int countDel = contextMatch.captured(2).toInt();
                      int startAdd = contextMatch.captured(3).toInt();
                      int countAdd = contextMatch.captured(4).toInt();
                      QString funcName = contextMatch.captured(5);
                      diffs.back().startContext(startDel, countDel, startAdd,
                                                countAdd, funcName);
                      hasContext = true;
                      return;
                  }

                  if (!hasContext)
                      return;

                  Q_ASSERT(!diffs.empty());
                  if (prefix[0] == '-') {
                      diffs.back().pushDeletedLine(noPrefix, newLine);
                      lastOp = DEL;
                  } else if (prefix[0] == '+') {
                      diffs.back().pushNewLine(noPrefix, newLine);
                      lastOp = ADD;
                  } else if (prefix[0] == ' ') {
                      diffs.back().pushLine(noPrefix, newLine);
                      lastOp = BOTH;
                  } else if (line == "\\ No newline at end of file") {
                      if (lastOp == ADD || lastOp == BOTH)
                          diffs.back().currentContext().setNoNewline(true);
                      if (lastOp == DEL || lastOp == BOTH)
                          diffs.back().currentContext().setNoNewline(false);
                  } else {
                      std::stringstream message;
                      message << "Unexpected first character in line ["
                              << line.toLatin1().data() << "]";
                      throw std::runtime_error(message.str());
                  }
              });

    return diffs;
}

Process Repository::fetch(QString remote) {
    if (remote.isEmpty()) {
        if (tracking_.first.isEmpty()) {
            // Nothing can be done !
            return Process();
        }

        remote = tracking_.first;
    }

    auto fetch = std::make_shared<GitFetch>(remote, root_);
    return Process(fetch, true);
}

bool Repository::initialise() {
    // Check if repository is valid
    if (!root_.exists()) {
        return false;
    }

    // Current branch
    {
        GitProcess process(root_);
        process.run(GitProcess::RevParse, QStringList() << "--abbrev-ref"
                                                        << "HEAD",
                    true, false);
        currentBranch_ = process.linesOut()[0];
        if (currentBranch_.isEmpty())
            return false;
    }

    {
        GitProcess process(root_);
        process.run(GitProcess::RevParse,
                    QStringList() << "--abbrev-ref"
                                  << (currentBranch_ + "@{upstream}"),
                    true, false);
        QString full;
        auto parts = process.linesOut();
        if (!parts.empty())
            full = parts[0];
        if (!full.isEmpty()) {
            auto first = full.indexOf("/");
            tracking_.first = full.left(first);
            tracking_.second = full.mid(first + 1);
        }
    }

    return true;
}

Process Repository::merge(QString branch) {
    if (branch.isEmpty())
        return Process();

    auto merge = std::make_shared<GitMerge>(branch, root_);
    return Process(merge, true);
}

std::unique_ptr<GitLogger> Repository::prepareGitLogger() const {
    return std::unique_ptr<GitLogger>(new GitLogger(root_));
}

Process Repository::pull(QString remote, QString branch) {
    if (remote.isEmpty()) {
        if (tracking_.first.isEmpty()) {
            // Nothing can be done !
            return Process();
        }

        remote = tracking_.first;
    }

    if (branch.isEmpty())
        return Process();

    auto fetch = std::make_shared<GitFetch>(remote, root_);
    auto merge = std::make_shared<GitMerge>(branch, root_);
    std::vector<std::shared_ptr<ICommand>> commands;
    commands.push_back(fetch);
    commands.push_back(merge);
    return Process(commands, true);
    /*
            if (!fetch(remote))
                    return false;

            if (remote.isEmpty())
                    remote = tracking_.first;
            ASSERT(!remote.isEmpty());

            if (branch.isEmpty())
            {
                    if (tracking_.second.isEmpty())
                    {
                            // Nothing can be done !
                            return false;
                    }

                    branch = tracking_.second;
            }

            return merge(remote + "/" + branch);*/
}

bool Repository::push(QString remote, QString branch) {
    if (remote.isEmpty()) {
        if (tracking_.first.isEmpty()) {
            // Nothing can be done !
            return false;
        }

        remote = tracking_.first;
    }

    if (branch.isEmpty()) {
        if (tracking_.second.isEmpty()) {
            // Nothing can be done !
            return false;
        }

        branch = tracking_.second;
    }

    GitProcess process(root_);
    return process.run(GitProcess::Push, QStringList() << remote << branch,
                       true, false);
    return true;
}

void Repository::stage(const FileStatus &file) const {
    GitProcess::eCommand command;
    switch (file.status(false)) {
    case FileStatus::ADDED:
    case FileStatus::MODIFIED:
        command = GitProcess::Add;
        break;
    case FileStatus::DELETED:
        command = GitProcess::Delete;
        break;
    default:
        throw std::runtime_error("Not implemented");
    }

    GitProcess process(root_);
    process.run(command, QStringList() << file.path().filePath(), false, false);
}

void Repository::stage(const DiffContext &context) const {
    GitProcess process(root_);
    process.runWithInput(GitProcess::Apply, QStringList() << "--cached"
                                                          << "-",
                         context.toPatch().toUtf8(), true, true);
}

void Repository::unstage(const FileStatus &file) const {
    GitProcess process(root_);
    process.run(GitProcess::Reset, QStringList() << file.path().filePath(),
                false, false);
}

bool Repository::updateStatus() {
    GitProcess process(root_);
    process.run(GitProcess::Status, QStringList() << "-b", true, false);
    QString output = process.out();

    // ?
    if (output == lastStatus_)
        return false;

    lastStatus_ = output;

    auto lines = process.linesOut();

    // 1st line: branch
    currentBranch_ = lines[0].mid(3);
    int branch_sep = currentBranch_.indexOf("...");
    if (branch_sep > 0)
        currentBranch_ = currentBranch_.left(branch_sep);

    files_.clear();

    // Subsequent lines: status [AMD? ][AMD? ] -> staged (added, modified,
    // deleted, ...), unstaged (added, modified, deleted, ...), filepath
    QStringList conflict;
    conflict << "DD"
             << "AU"
             << "UD"
             << "UA"
             << "DU"
             << "AA"
             << "UU";

    std::stack<QFileInfo> subdirs;

    for (int i = 1; i < lines.length(); ++i) {
        const bool has_conflict = (conflict.indexOf(lines[i].left(2)) >= 0);

        FileStatus::eStatus statIndex = FileStatus::NORMAL;
        FileStatus::eStatus statWorkTree = FileStatus::NORMAL;

        // Index (first): [MADRC ]
        if (lines[i][0] == 'M')
            statIndex = FileStatus::MODIFIED;
        else if (lines[i][0] == 'A')
            statIndex = FileStatus::ADDED;
        else if (lines[i][0] == 'D')
            statIndex = FileStatus::DELETED;
        else if (lines[i][0] == 'R')
            statIndex = FileStatus::RENAMED;
        else if (lines[i][0] == 'C')
            statIndex = FileStatus::COPIED;

        // Work tree (second): [MD ]
        if (lines[i][1] == 'M')
            statWorkTree = FileStatus::MODIFIED;
        else if (lines[i][1] == 'D')
            statWorkTree = FileStatus::DELETED;
        else if (lines[i][1] == '?')
            statWorkTree = FileStatus::ADDED;

        // Unmerged status: DD, AU, UD, UA, DU, AA, UU
        // ?? Untracked !! Ignored

        QString name = lines[i].mid(3);
        int rename = lines[i].indexOf(" -> ");
        if (rename >= 0)
            name = lines[i].mid(rename + 4);

        QFileInfo path(name);
        QFileInfo fullPath(root_.absolutePath() + QDir::separator() + name);

        // TODO: symlinks?
        if (statWorkTree == FileStatus::ADDED && fullPath.isDir()) {
            // Look for files inside that directory
            subdirs.push(path);
            continue;
        }

        files_.insert(FileStatus(path, statIndex, statWorkTree, has_conflict));
    }

    // Now add files in new directories...
    while (!subdirs.empty()) {
        GitProcess process(root_);
        process.run(GitProcess::Status,
                    QStringList() << (subdirs.top().filePath() + "*"), true,
                    false);
        lines = process.linesOut();

        subdirs.pop();

        for (auto line : lines) {
            QString name = line.mid(3);
            QFileInfo path(name);
            if (path.isDir()) {
                subdirs.push(path);
                continue;
            }
            files_.insert(
                FileStatus(name, FileStatus::NORMAL, FileStatus::ADDED, false));
        }
    }

    return true;
}
}
