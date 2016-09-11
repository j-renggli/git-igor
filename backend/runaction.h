#pragma once

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QStringList>

/*
#include <QtCore/QThread>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stack>
#include <thread>
*/
namespace gitkit {

class ICommand
{
	public:
		virtual QString command() const = 0;
		virtual bool run() = 0;
		virtual QString stdout() const = 0;
		virtual QString stderr() const = 0;
};

class RunCommand : public ICommand
{
	public:
		RunCommand(const QString& command, const QStringList& args, const QDir& workDir)
		: command_(command)
		, arguments_(args)
		, workDir_(workDir)
		{}
		~RunCommand() {}
		
	protected:
		QString command() const override;
		bool run() override;
		QString stdout() const override { return output_; }
		QString stderr() const override { return error_; }
		
	private:
		QString command_;
		QStringList arguments_;
		QDir workDir_;
		
		QString output_;
		QString error_;
};

class GitAction : public RunCommand
{
	public:
		GitAction(const QStringList& args, const QDir& workDir)
		: RunCommand("git", args, workDir)
		{}
		~GitAction() {}
};

class GitFetch : public GitAction
{
	public:
		GitFetch(const QString& remote, const QDir& workDir)
		: GitAction(QStringList() << "fetch" << remote, workDir)
		{}
		~GitFetch() {}
};

class GitMerge : public GitAction
{
	public:
		GitMerge(const QString& branch, const QDir& workDir)
		: GitAction(QStringList() << "merge" << branch, workDir)
		{}
		~GitMerge() {}
};
/*
class Process
{
	public:
		virtual ~Process() {}
		Process() {}
		Process(const std::shared_ptr<ICommand>& command);
		
	public:
		virtual bool empty() const;
		virtual std::shared_ptr<ICommand> next();
		
	private:
		std::stack<std::shared_ptr<ICommand> > commands_;
};
*/
}