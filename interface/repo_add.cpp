#include "repo_add.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

namespace gitkit {
	
UIRepoAdd::UIRepoAdd()
{
	setModal(true);
}

UIRepoAdd::~UIRepoAdd()
{
}
	
void UIRepoAdd::initialise(bool withPath)
{
	auto layout = new QGridLayout;
	
	int row = 0;
	{
		QLabel* lblName = new QLabel("Name", this);
		layout->addWidget(lblName, row, 0, 1, 1);
		name_ = new QLineEdit(this);
		layout->addWidget(name_, row, 1, 1, 2);
	}
	
	if (withPath)
	{
		++row;
		QLabel* lblPath = new QLabel("Path", this);
		layout->addWidget(lblPath, row, 0, 1, 1);
		path_ = new QLineEdit(this);
		layout->addWidget(path_, row, 1, 1, 1);
		pathButton_ = new QPushButton("...", this);
		layout->addWidget(pathButton_, row, 2, 1, 1);
	}
	
	{
		++row;
		QLabel* lblRemote = new QLabel("Remote", this);
		layout->addWidget(lblRemote, row, 0, 1, 1);
		remote_ = new QLineEdit(this);
		layout->addWidget(remote_, row, 1, 1, 1);
		remoteButton_ = new QPushButton("...", this);
		layout->addWidget(remoteButton_, row, 2, 1, 1);
		
		++row;
		initRemote_ = new QCheckBox("Init remote", this);
		layout->addWidget(initRemote_, row, 1, 1, 2);
	}
	
	{
		++row;
		info_ = new QTextEdit(this);
		layout->addWidget(info_, row, 0, 1, 3);
	}
	
	{
		++row;
		auto btnLayout = new QHBoxLayout;
		cancel_ = new QPushButton("Cancel", this);
		btnLayout->addWidget(cancel_);
		commit_ = new QPushButton("Commit", this);
		btnLayout->addWidget(commit_);
		auto btnFrame = new QFrame;
		btnFrame->setLayout(btnLayout);
		layout->addWidget(btnFrame, row, 0, 1, 3);
	}
	
	connect(name_, &QLineEdit::textChanged, this, &UIRepoAdd::onNameModified);
	//connect(repositories_, &QTreeView::clicked, this, &UIRepositories::onSelecting);
	connect(cancel_, &QPushButton::clicked, this, &UIRepoAdd::reject);
	//connect(commit_, &QPushButton::clicked, this, &UIRepositories::onCommit);
	//connect(repoAdd_, &QPushButton::clicked, this, &UIRepositories::onAddRepo);
	
	setLayout(layout);
	setWindowTitle(withPath ? "Add Repository" : "Add Remote");
}

void UIRepoAdd::onNameModified(const QString& text)
{
}
	
void UIRepoAdd::onPathModified()
{
	updateUI();}

void UIRepoAdd::onSourceModified()
{
	updateUI();
}

void UIRepoAdd::updateUI()
{
}

}
