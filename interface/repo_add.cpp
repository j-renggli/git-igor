#include "repo_add.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>


#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <iostream>

namespace gitigor {
	
UIRepoAdd::UIRepoAdd(bool isRepository)
: isRepository_(isRepository)
{
	setModal(true);
}

UIRepoAdd::~UIRepoAdd()
{
}
	
void UIRepoAdd::initialise()
{
	auto layout = new QGridLayout;
	
	int row = 0;
	{
		QLabel* lblName = new QLabel("Name", this);
		layout->addWidget(lblName, row, 0, 1, 1);
		name_ = new QLineEdit(this);
		layout->addWidget(name_, row, 1, 1, 2);
	}
	
	if (isRepository_)
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
		initRemote_->setCheckState(Qt::Checked);
		layout->addWidget(initRemote_, row, 1, 1, 2);
	}
	
	{
		++row;
		info_ = new QTextEdit(this);
		info_->setReadOnly(true);
		layout->addWidget(info_, row, 0, 1, 3);
	}
	
	{
		++row;
		auto btnLayout = new QHBoxLayout;
		cancel_ = new QPushButton("Cancel", this);
		btnLayout->addWidget(cancel_);
		commit_ = new QPushButton("Add", this);
		btnLayout->addWidget(commit_);
		auto btnFrame = new QFrame;
		btnFrame->setLayout(btnLayout);
		layout->addWidget(btnFrame, row, 0, 1, 3);
	}
	
	chooseFolder_ = new QFileDialog(this, "Chose folder", QDir::currentPath());
	chooseFolder_->setFileMode(QFileDialog::Directory);
	chooseFolder_->setOption(QFileDialog::ShowDirsOnly, true);
	chooseFolder_->setModal(true);
	
	connect(name_, &QLineEdit::textChanged, this, &UIRepoAdd::onNameModified);
	if (isRepository_)
	{
		connect(path_, &QLineEdit::textChanged, this, &UIRepoAdd::onPathModified);
		connect(pathButton_, &QPushButton::clicked, this, &UIRepoAdd::onSelectPath);
	}
	connect(remote_, &QLineEdit::textChanged, this, &UIRepoAdd::onRemoteModified);
	connect(remoteButton_, &QPushButton::clicked, this, &UIRepoAdd::onSelectRemote);
	connect(cancel_, &QPushButton::clicked, this, &UIRepoAdd::reject);
	connect(commit_, &QPushButton::clicked, this, &UIRepoAdd::onCommit);
	//connect(repoAdd_, &QPushButton::clicked, this, &UIRepositories::onAddRepo);
	
	setLayout(layout);
	setWindowTitle(isRepository_ ? "Add Repository" : "Add Remote");
	
	updateUI();
}
	
QString UIRepoAdd::name() const
{
	return name_->text();
}

QDir UIRepoAdd::path() const
{
	return path_->text();
}

void UIRepoAdd::onCommit()
{
	accept();
}

void UIRepoAdd::onNameModified(const QString& text)
{
	updateUI();
}
	
void UIRepoAdd::onPathModified(const QString& text)
{
	updateUI();}

void UIRepoAdd::onRemoteModified(const QString& text)
{
	updateUI();
}

void UIRepoAdd::updateUI()
{
	bool ok = true;
	
	QString text;
	if (name_->text().isEmpty())
	{
		text  = "<p class=\"error\">Please enter a name for the new ";
		text += (isRepository_ ? "repository" : "remote");
		text += ".</p>";
		ok = false;
	}
	else if (isRepository_)
	{
		if (path_->text().isEmpty())
		{
			text = "<p class=\"error\">Please select a path on disk for the new repository.</p>";
			ok = false;
		}
		else
		{
			QUrl asUrl = QUrl::fromUserInput(path_->text());
			if (asUrl.isLocalFile())
			{
				QDir path = asUrl.toLocalFile();
				if (path.exists())
				{
					while (true)
					{
						if (path.isRoot())
							break;
						
						if (path.exists(".git"))
							break;
							
						if (!path.cdUp())
							break;
					}
					
					if (path.exists(".git"))
					{
						text  = "<p class=\"info\">Will add existing git repository ";
						text += path.absolutePath();
						text += ".</p>";
					}
				}
				else
				{
					text  = "<p class=\"info\">Will create new git repository ";
					text += path.absolutePath();
					text += ".</p>";
				}
			}
			else
			{
				text  = "<p class=\"error\">";
				text += asUrl.path();
				text += " is not a local repository.</p>";
				ok = false;
			}
		}
	}
	else if (remote_->text().isEmpty())
	{
		text  = "<p class=\"error\">Please enter a url for the new remote.</p>";
		ok = false;
	}
	
	bool showInitRemote = false;
	if (!remote_->text().isEmpty())
	{
		QUrl asUrl = QUrl::fromUserInput(remote_->text());
		if (asUrl.isLocalFile())
		{
			QDir path = asUrl.toLocalFile();
			if (!path.exists())
				showInitRemote = true;
		}
	}
	
	initRemote_->setVisible(showInitRemote);
	
	text = "<html><head><style>body { background-color: rgba(255, 255, 255, 50); } .error { color: red; } </style></head><body>" + text + "</body></html>";
	info_->setHtml(text);
	
	commit_->setEnabled(ok);
}

void UIRepoAdd::onSelectPath()
{
	auto accepted = chooseFolder_->exec();
	if (accepted == QFileDialog::Accepted)
	{
		auto urls = chooseFolder_->selectedUrls();
		if (urls.length() == 1)
			path_->setText(urls[0].path());
	}
}

void UIRepoAdd::onSelectRemote()
{
	chooseFolder_->exec();
	auto accepted = chooseFolder_->result();
	if (accepted == QFileDialog::Accepted)
	{
		auto urls = chooseFolder_->selectedUrls();
		if (urls.length() == 1)
			remote_->setText(urls[0].path());
	}
}

}
