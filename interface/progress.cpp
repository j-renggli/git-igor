#include "progress.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

#include <backend/backend.h>
#include <backend/runaction.h>

namespace gitigor {

UIProgress::UIProgress()
{
}

UIProgress::~UIProgress()
{
}
	
void UIProgress::initialise()
{
	auto layout = new QVBoxLayout;
	
	command_ = new QLineEdit();
	command_->setReadOnly(true);
	output_ = new QTextEdit();
	output_->setReadOnly(true);
	progress_ = new QProgressBar();
	progress_->setMinimum(0);
	progress_->setMaximum(0);
	
	layout->addWidget(command_);
	layout->addWidget(progress_);
	layout->addWidget(output_);
	// TODO: buttons
	/*
	auto buttons = new QHBoxLayout;
	buttons->addWidget(commit_);
	buttons->addWidget(pullCommitPush_);
	layout->addLayout(buttons);
	*/
	//auto frame = new QWidget;
	setLayout(layout);
	//setWidget(frame);
	setWindowTitle("Progress");
	
	Backend& backend = Backend::instance();
	const ActionRunner& runner = backend.taskRunner();
	connect(&runner, &ActionRunner::notifyShowDialog, this, &UIProgress::onShow);
	connect(&runner, &ActionRunner::notifyStartWork, this, &UIProgress::onSetCommand);
	connect(&runner, &ActionRunner::notifyOutput, this, &UIProgress::onOutput);
	connect(&runner, &ActionRunner::notifyStopWork, this, &UIProgress::onWorkDone);
}

void UIProgress::onShow()
{
	show();
}

void UIProgress::onSetCommand(const QString& command)
{
	command_->setText(command);
}

void UIProgress::onOutput(const QString& output)
{
	output_->setText(output);
}

void UIProgress::onWorkDone()
{
	hide();
}

}
