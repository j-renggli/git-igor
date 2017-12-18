#pragma once

#include <set>

#include <QtCore/QDir>

#include "diff.h"
#include "runner.h"

namespace gitkit {
	
class FileStatus
{
	public:
		enum eStatus
		{
			NORMAL,
			ADDED,
			MODIFIED,
			DELETED,
			RENAMED,
			COPIED
		};
		
	public:
		FileStatus(const QFileInfo& path, eStatus index, eStatus workTree, bool conflict);
		
		bool operator<(const FileStatus& rhs) const;
		
		QFileInfo path() const { return path_; }
		eStatus status(bool indexed) const { return indexed ? index_ : workTree_; }
	
	private:
		QFileInfo path_; // Rename -> renamed path !?
		eStatus index_;
		eStatus workTree_;
		bool conflict_;
};

class Repository
{
	public:
		Repository(const QString& name, const QDir& root);
		
	public:
		bool initialise();
		
		bool commit(const QString& message) const;
		std::vector<Diff> diff(const FileStatus& file, bool indexed) const;
		void stage(const FileStatus& file) const;
		void unstage(const FileStatus& file) const;
		bool updateStatus();
		
		Process fetch(QString remote = "");
		Process pull(QString remote = "", QString branch = "");
		Process merge(QString branch);
		bool push(QString remote = "", QString branch = "");
		
		const QString& name() const { return name_; }
		const QDir root() const { return root_; }
		const std::vector<QString>& remotes() const { return remotes_; }
		const std::set<FileStatus>& inStage() const { return files_; }
		
	private:
		Repository(const Repository& copy);

        bool updateStatus(const QDir& path);
		
	private:
		QDir root_;
		QString name_;
		
		QString lastStatus_;
		
		QString currentBranch_;
		
		std::set<FileStatus> files_;
		
		std::vector<QString> remotes_;
		std::vector<QString> branches_;
		std::pair<QString, QString> tracking_;

		static const QRegularExpression s_rxDiffFiles;
		static const QRegularExpression s_rxDiffContext;
};

}
