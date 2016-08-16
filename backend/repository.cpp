#include "repository.h"

#include <cassert>
#include <iostream>
#include <stack>

#include <QtCore/QProcess>
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

namespace gitkit {
	
const QRegExp Repository::s_rxLineEnd("[\r\n]");
const QRegularExpression Repository::s_rxDiffFiles("^diff --git a[/](.+) b[/](.+)$");
const QRegularExpression Repository::s_rxDiffContext("^@@ [-](\\d+),(\\d+) [+](\\d+),(\\d+) @@ (.+)$");

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

Diff::Diff(const QFileInfo& left, const QFileInfo& right)
: left_(left)
, right_(right)
{
}

void Diff::addLine(const QString& line)
{
	lines_ << line;
}

////////////////////////////////////////////////////////////////

bool Repository::commit(const QString& message) const
{
	if (message.isEmpty())
		return false;
		
	QProcess process;
	process.start("git", QStringList() << "commit" << "-m" << message);
	process.waitForFinished();
	return true;
}

std::vector<Diff> Repository::diff(const FileStatus& file, bool indexed) const
{
	std::vector<Diff> diffs;
	
	// New file ? Whole file added !
	if (file.status(false) == FileStatus::ADDED)
	{
		QFile disk_file( file.path().filePath() );
		if ( disk_file.open(QIODevice::ReadOnly) )
		{
			QTextStream in(&disk_file);
			
			diffs.push_back(Diff(QFileInfo(), file.path()));

			while(!in.atEnd())
			{
				diffs.back().addLine("+" + in.readLine());
			}

			disk_file.close();
		}
		
		return diffs;
	}
	
	QProcess process;
	QStringList args;
	args << "diff" << "--no-color";
	if (indexed)
		args << "--cached";
		
	args << file.path().filePath();
	process.start("git", args);
	process.waitForFinished();
	QString output = process.readAllStandardOutput();
	auto lines = output.split(s_rxLineEnd, QString::SkipEmptyParts);
	
	QFileInfo left, right;
	QString context;
	
	for (auto line : lines)
	{
		//std::cout << line.toLatin1().data() << std::endl;
		QRegularExpressionMatch match = s_rxDiffFiles.match(line);
		if (match.hasMatch())
		{
			left = match.captured(1);
			right = match.captured(2);
			context = "";
			diffs.push_back(Diff(left, right));
			continue;
		}
		
		match = s_rxDiffContext.match(line);
		if (match.hasMatch())
		{
			context = match.captured(5);
			continue;
		}
		
		if (context.isEmpty())
			continue;
			
		assert(!diffs.empty());
		diffs.back().addLine(line);
	}
	
	return diffs;
}

void Repository::stage(const FileStatus& file) const
{
	QProcess process;
	QStringList args;
	switch (file.status(false))
	{
	case FileStatus::ADDED:
	case FileStatus::MODIFIED:
		args << "add" << file.path().filePath();
		break;
	case FileStatus::DELETED:
		args << "rm" << file.path().filePath();
		break;
	default:
		std::cerr << "Not implemented" << std::endl;
		return;
	}
	
	process.start("git", args);
	process.waitForFinished();
}

void Repository::unstage(const FileStatus& file) const
{
	QProcess process;
	QStringList args;
	args << "reset" << file.path().filePath();
	process.start("git", args);
	process.waitForFinished();
}

bool Repository::updateStatus()
{
	QProcess process;
	process.start("git", QStringList() << "status" << "--porcelain" << "-b");
	process.waitForFinished();
	QString output = process.readAllStandardOutput();
	
	// ?
	if (output == lastStatus_)
		return false;
		
	lastStatus_ = output;
	
	auto lines = output.split(s_rxLineEnd, QString::SkipEmptyParts);
	
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
			
		// TODO: symlinks?
		if (statWorkTree == FileStatus::ADDED && path.isDir())
		{
			// Look for files inside that directory
			subdirs.push(path);
			continue;
		}
			
		//std::cout << path.toLatin1().data() << ": " << statIndex << ", " << statWorkTree << std::endl;
		files_.insert(FileStatus(path, statIndex, statWorkTree, has_conflict));
	}
	
	// Now add files in new directories...
	while (!subdirs.empty())
	{
		//std::cout << subdirs.top().filePath().toLatin1().data() << std::endl;
		process.start("git", QStringList() << "status" << (subdirs.top().filePath() + "*") << "--porcelain");
		process.waitForFinished();
		QString output = process.readAllStandardOutput();
		
		lines = output.split(s_rxLineEnd, QString::SkipEmptyParts);
		//std::cout << output.toLatin1().data() << std::endl;
		subdirs.pop();
		
		for (auto line : lines)
		{
			QString name = line.mid(3);
			files_.insert(FileStatus(name, FileStatus::NORMAL, FileStatus::ADDED, false));
		}	}
	
	return true;
}

}