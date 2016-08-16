#pragma once

#include <set>

#include <QtCore/QDir>

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

class Repository
{
	public:
		
	public:
		bool commit(const QString& message) const;
		std::vector<Diff> diff(const FileStatus& file, bool indexed) const;
		void stage(const FileStatus& file) const;
		void unstage(const FileStatus& file) const;
		bool updateStatus();
		
		void fetch(const QString& remote = "");
		void pull(const QString& remote = "", const QString& branch = "");
		void merge(const QString& branch);
		void push(const QString& remote = "", const QString& branch = "");
		
		const std::set<FileStatus>& inStage() const { return files_; }
		
	private:
		QDir root_;
		QString name_;
		
		QString lastStatus_;
		
		QString currentBranch_;
		/*
		QString currentRemote_;
		*/
		
		std::set<FileStatus> files_;
		
		static const QRegExp s_rxLineEnd;
		static const QRegularExpression s_rxDiffFiles;
		static const QRegularExpression s_rxDiffContext;
};

}