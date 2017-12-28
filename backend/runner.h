#pragma once

#include <QtCore/QThread>
/*
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <thread>
*/
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>

#include "runaction.h"

namespace gitigor {
/*
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
		RunCommand(const QString& command, const QStringList& args, const QDir& cwd)
		: command_(command)
		, arguments_(args)
		, cwd_(cwd)
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
		QDir cwd_;
		
		QString output_;
		QString error_;
};
*/
class Process
{
	public:
		virtual ~Process() {}
		Process() {}
		Process(const std::shared_ptr<ICommand>& command, bool showDialog);
		Process(const std::vector<std::shared_ptr<ICommand> >& commands, bool showDialog);
		
	public:
		virtual bool empty() const;
		virtual std::shared_ptr<ICommand> next();
		virtual bool showDialog() const { return showDialog_; }
		
	private:
		bool showDialog_;
		std::stack<std::shared_ptr<ICommand> > commands_;
};

class ActionWorker : public QThread
{
	Q_OBJECT
	
	public:
		ActionWorker();
	
	public:
		static void enqueue(const Process& process);
		
	public:
		void run() Q_DECL_OVERRIDE;
		
	signals:
		void notifyStartWork(const QString& command);
		void notifyOutput(const QString& currentOutput);
		void notifyError(const QString& error);
		void notifyStopWork();
		void notifyShowDialog();
			
	private:
		bool stop_;
		
	private:
		static std::mutex s_mutex;
		static std::condition_variable s_condition;
		static std::queue<Process> s_processes;
};
	
class ActionRunner : public QObject
{
	Q_OBJECT
	
	public:
		ActionRunner();
		~ActionRunner();
	
	public:
		bool enqueue(const Process& process);
		bool initialise();
		
	signals:
		void notifyStartWork(const QString& command);
		void notifyOutput(const QString& currentOutput);
		void notifyError(const QString& error);
		void notifyStopWork();
		void notifyShowDialog();
			
	private:
		ActionWorker* pWorker_;
};

}