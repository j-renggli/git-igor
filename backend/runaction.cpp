#include "runaction.h"

#include <QtCore/QProcess>

#include <iostream>

namespace gitigor {

QString RunCommand::command() const
{
	if (arguments_.isEmpty())
		return command_;
	return command_ + " " + arguments_.join(" ");
}

bool RunCommand::run()
{
	QProcess process;
	process.setProgram(command_);
	process.setArguments(arguments_);
	if (workDir_.exists())
		process.setWorkingDirectory(workDir_.absolutePath());
	process.start();
	process.waitForFinished();
	if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0)
	{
		output_ = process.readAllStandardError();
		return false;
	}
	else
	{
		output_ = process.readAllStandardOutput();
		return true;
	}
}

}
