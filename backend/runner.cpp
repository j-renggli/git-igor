#include "runner.h"

#include <QtCore/QProcess>

#include <iostream>

#include <gkassert.h>

namespace gitkit {

Process::Process(const std::shared_ptr<ICommand>& command, bool showDialog)
: showDialog_(showDialog)
{
	commands_.push(command);
}
	
Process::Process(const std::vector<std::shared_ptr<ICommand> >& commands, bool showDialog)
: showDialog_(showDialog)
{
	for (auto r_it = commands.rbegin(); r_it != commands.rend(); ++r_it)
	{
		commands_.push(*r_it);
	}
}
	
bool Process::empty() const
{
	return commands_.empty();
}

std::shared_ptr<ICommand> Process::next()
{
	if (commands_.empty())
		return std::shared_ptr<ICommand>();
		
	auto next = commands_.top();
	commands_.pop();
	return next;
}

std::mutex ActionWorker::s_mutex;
std::condition_variable ActionWorker::s_condition;
std::queue<Process> ActionWorker::s_processes;

ActionWorker::ActionWorker()
: stop_(false)
{
}

void ActionWorker::enqueue(const Process& process)
{
	ASSERT(!process.empty());
		
	std::lock_guard<std::mutex> lock(s_mutex);
	
	s_processes.push(process);
	
	s_condition.notify_all(); // notify_one ?
}

void ActionWorker::run()
{
	while (true)
	{
		// Don't wan't multiple access
		std::unique_lock<std::mutex> lock(s_mutex);
		
		if (stop_)
			return;

		// Wait until event happens
		while (s_processes.empty())
		{
			if (stop_)
				return;
				
			s_condition.wait(lock); // Releases the lock, regain it as soon as notified !
		}

		// Ok, we have an event, process it !
		Process next = s_processes.front();
		s_processes.pop();
		
		if (next.showDialog())
		{
			emit notifyShowDialog();
		}
		
		while (true)
		{
			auto cmd = next.next();
			if (!cmd)
				break;
				
			emit notifyStartWork(cmd->command());
			
			if (cmd->run())
			{
				emit notifyOutput(cmd->stdout());
			}
			else
			{
				emit notifyError(cmd->stderr());
			}
		}
		
		emit notifyStopWork();
	}
}

ActionRunner::ActionRunner()
{
	pWorker_ = new ActionWorker;
}

ActionRunner::~ActionRunner()
{
}

bool ActionRunner::enqueue(const Process& process)
{
	// TODO: Check if thread running
	
	pWorker_->enqueue(process);
	return true;
}

bool ActionRunner::initialise()
{
	connect(pWorker_, &QThread::finished, pWorker_, &QObject::deleteLater);
	connect(pWorker_, &ActionWorker::notifyStartWork, this, &ActionRunner::notifyStartWork);
	connect(pWorker_, &ActionWorker::notifyStopWork, this, &ActionRunner::notifyStopWork);
	connect(pWorker_, &ActionWorker::notifyOutput, this, &ActionRunner::notifyOutput);
	connect(pWorker_, &ActionWorker::notifyError, this, &ActionRunner::notifyError);
	connect(pWorker_, &ActionWorker::notifyShowDialog, this, &ActionRunner::notifyShowDialog);
	pWorker_->start();

	return true;
}

}