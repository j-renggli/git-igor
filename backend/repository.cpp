#include "repository.h"

#include <iostream>
#include <stack>
#include <sstream>

#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

#include <gkassert.h>

#include "gitprocess.h"
#include "runaction.h"

namespace gitkit {

const QRegularExpression Repository::s_rxDiffFiles("^diff --git a[/](.+) b[/](.+)$");
const QRegularExpression Repository::s_rxDiffContext("^@@ [-](\\d+),(\\d+) [+](\\d+),(\\d+) @@(?: (.+))?$");

FileStatus::FileStatus(const QFileInfo& path, eStatus index, eStatus workTree, bool conflict)
: path_(path)
, index_(index)
, workTree_(workTree)
, conflict_(conflict)
{
}

bool FileStatus::operator<(const FileStatus& rhs) const
{
	return path_.filePath() < rhs.path_.filePath();
}

////////////////////////////////////////////////////////////////

Repository::Repository(const QString& name, const QDir& root)
: root_(root)
, name_(name)
{
}

Repository::Repository(const Repository& copy)
{
	ASSERT(false);
}

bool Repository::commit(const QString& message) const
{
    if (message.isEmpty())
        return false;

    GitProcess process(root_);
    return process.run(GitProcess::Commit, QStringList() << "-m" << message, false, false);
}

std::vector<Diff> Repository::diff(const FileStatus& file, bool indexed) const
{
	std::vector<Diff> diffs;
	
	// New file ? Whole file added !
	FileStatus::eStatus workTreeStatus = file.status(false);
	if (workTreeStatus == FileStatus::ADDED)
    {
        QFile disk_file( root_.filePath(file.path().filePath()) );
		if ( disk_file.open(QIODevice::ReadOnly) )
		{
			QTextStream in(&disk_file);
			
			diffs.push_back(Diff(QFileInfo(), file.path()));

			while(!in.atEnd())
			{
                diffs.back().pushNewLine(in.readLine());
			}

			disk_file.close();
		}
		
		return diffs;
	}
	else if (workTreeStatus == FileStatus::DELETED)
	{
        QString head("HEAD:");
        head += file.path().filePath();

        GitProcess process(root_);
        process.run(GitProcess::Show, QStringList() << head, true, false);
        auto lines = process.linesOut();

		diffs.push_back(Diff(file.path(), file.path()));
		for (auto line : lines)
            diffs.back().pushDeletedLine(line);
		return diffs;
	}

    QStringList args;
    args << "--no-color";
    if (indexed)
        args << "--cached";
    args << file.path().filePath();

    GitProcess process(root_);
    process.run(GitProcess::Diff, args, true, false);
    auto lines = process.linesOut();

	QFileInfo left, right;
	QString context;
	bool hasContext = false;
	
    enum Previous
    {
        ADD,
        DEL,
        BOTH
    };
    Previous lastOp = BOTH;

	for (auto line : lines)
	{
		//std::cout << line.toLatin1().data() << std::endl;
		QRegularExpressionMatch match = s_rxDiffFiles.match(line);
		if (match.hasMatch())
		{
			left = match.captured(1);
			right = match.captured(2);
			context = "";
			hasContext = false;
            diffs.push_back(Diff(left, right));
			continue;
        }

        match = s_rxDiffContext.match(line);
        if (match.hasMatch())
        {
            int startDel = match.captured(1).toInt();
            int countDel = match.captured(2).toInt();
            int startAdd = match.captured(3).toInt();
            int countAdd = match.captured(4).toInt();
            QString funcName = match.captured(5);
            diffs.back().startContext(startDel, countDel, startAdd, countAdd, funcName);
            hasContext = true;
            continue;
        }

		if ( !hasContext )
			continue;
			
		ASSERT(!diffs.empty());
        if (line[0] == '-') {
            diffs.back().pushDeletedLine(line.mid(1));
            lastOp = DEL;
        } else if (line[0] == '+') {
            diffs.back().pushNewLine(line.mid(1));
            lastOp = ADD;
        } else if (line[0] == ' ') {
            diffs.back().pushLine(line.mid(1));
            lastOp = BOTH;
        } else if (line == "\\ No newline at end of file") {
            if (lastOp == ADD || lastOp == BOTH)
                diffs.back().currentContext().setNoNewline(true);
            if (lastOp == DEL || lastOp == BOTH)
                diffs.back().currentContext().setNoNewline(false);
        } else {
            std::stringstream message;
            message << "Unexpected first character in line [" << line.toLatin1().data() << "]";
            throw std::runtime_error(message.str());
        }
	}
	
	return diffs;
}

Process Repository::fetch(QString remote)
{
	if (remote.isEmpty())
	{
		if (tracking_.first.isEmpty())
		{
			// Nothing can be done !
			return Process();
		}
		
		remote = tracking_.first;
	}
	
	auto fetch = std::make_shared<GitFetch>(remote, root_);
	return Process(fetch, true);
}

bool Repository::initialise()
{
	// Current branch
	{
        GitProcess process(root_);
        process.run(GitProcess::RevParse, QStringList() << "--abbrev-ref" << "HEAD", true, false);
        currentBranch_ = process.linesOut()[0];
		if (currentBranch_.isEmpty())
			return false;
	}
	
    {
        GitProcess process(root_);
        process.run(GitProcess::RevParse, QStringList() << "--abbrev-ref" << (currentBranch_ + "@{upstream}"), true, false);
        QString full;
        auto parts = process.linesOut();
        if (!parts.empty())
            full = parts[0];
		if (!full.isEmpty())
		{
			auto first = full.indexOf("/");
			tracking_.first = full.left(first);
			tracking_.second = full.mid(first+1);
		}
	}
	
	return true;
}

Process Repository::merge(QString branch)
{
	if (branch.isEmpty())
		return Process();
		
	auto merge = std::make_shared<GitMerge>(branch, root_);
	return Process(merge, true);
}

Process Repository::pull(QString remote, QString branch)
{
	if (remote.isEmpty())
	{
		if (tracking_.first.isEmpty())
		{
			// Nothing can be done !
			return Process();
		}
		
		remote = tracking_.first;
	}
	
	if (branch.isEmpty())
		return Process();
		
	auto fetch = std::make_shared<GitFetch>(remote, root_);
	auto merge = std::make_shared<GitMerge>(branch, root_);
	std::vector<std::shared_ptr<ICommand> > commands;
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

bool Repository::push(QString remote, QString branch)
{
	if (remote.isEmpty())
	{
		if (tracking_.first.isEmpty())
		{
			// Nothing can be done !
			return false;
		}
		
		remote = tracking_.first;
	}
	
	if (branch.isEmpty())
	{
		if (tracking_.second.isEmpty())
		{
			// Nothing can be done !
			return false;
		}
		
		branch = tracking_.second;
    }

    GitProcess process(root_);
    return process.run(GitProcess::Push, QStringList() << remote << branch, true, false);
    std::cout << "Push: [" << process.out().toLatin1().data() << "]" << std::endl;
	return true;
}

void Repository::stage(const FileStatus& file) const
{
    GitProcess::eCommand command;
	switch (file.status(false))
	{
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

void Repository::unstage(const FileStatus& file) const
{
    GitProcess process(root_);
    process.run(GitProcess::Reset, QStringList() << file.path().filePath(), false, false);
}

bool Repository::updateStatus()
{
    GitProcess process(root_);
    process.run(GitProcess::Status, QStringList() << "--porcelain" << "-b", true, false);
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
		
	// Subsequent lines: status [AMD? ][AMD? ] -> staged (added, modified, deleted, ...), unstaged (added, modified, deleted, ...), filepath
	QStringList conflict;
	conflict << "DD" << "AU" << "UD" << "UA" << "DU" << "AA" << "UU";
	
	std::stack<QFileInfo> subdirs;
	
	for (int i = 1; i < lines.length(); ++i)
	{
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
			name = lines[i].mid(rename+4);
			
        QFileInfo path(name);
        QFileInfo fullPath(root_.absolutePath() + QDir::separator() + name);
        std::cout << "File " << name.toLatin1().data() << ": " << statWorkTree << ", " << fullPath.isDir() << std::endl;

        // TODO: symlinks?
        if (statWorkTree == FileStatus::ADDED && fullPath.isDir())
        {
            // Look for files inside that directory
            std::cout << "Subdir " << name.toLatin1().data() << std::endl;
            subdirs.push(path);
            continue;
        }

        //std::cout << path.toLatin1().data() << ": " << statIndex << ", " << statWorkTree << std::endl;
        files_.insert(FileStatus(path, statIndex, statWorkTree, has_conflict));
    }

	// Now add files in new directories...
	while (!subdirs.empty())
	{
        GitProcess process(root_);
        process.run(GitProcess::Status, QStringList() << (subdirs.top().filePath() + "*") << "--porcelain", true, false);
        lines = process.linesOut();

		subdirs.pop();
		
		for (auto line : lines)
		{
			QString name = line.mid(3);
			QFileInfo path(name);
			if (path.isDir())
			{
				subdirs.push(path);
				continue;
			}
			files_.insert(FileStatus(name, FileStatus::NORMAL, FileStatus::ADDED, false));
		}
	}
	
	return true;
}

}
