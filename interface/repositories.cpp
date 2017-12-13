#include "repositories.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeView>

#include <gkassert.h>
#include <backend/repositorymanager.h>

#include "repo_add.h"

namespace gitkit {
	
UIRepositories::UIRepositories()
: selected_(-1)
{
	setModal(true);
}

UIRepositories::~UIRepositories()
{
}
	
void UIRepositories::initialise()
{
	auto layout = new QGridLayout;
	
	int leftCol = 0;
	int leftSize = 4;
	int rightCol = leftCol+leftSize;
	int rightSize = 3;
	
	auto& manager = RepositoryManager::instance();
	
	repositories_ = new QTreeView;
	repositories_->setModel(&manager);
	layout->addWidget(repositories_, 0, leftCol, 4, leftSize);
	
	{
		int btnRow = 4;
		int btnCol = leftCol;
		repoDel_ = new QPushButton("Remove");
		layout->addWidget(repoDel_, btnRow, btnCol++, 1, 1);
		repoDown_ = new QPushButton("Down");
		layout->addWidget(repoDown_, btnRow, btnCol++, 1, 1);
		repoUp_ = new QPushButton("Up");
		layout->addWidget(repoUp_, btnRow, btnCol++, 1, 1);
		repoAdd_ = new QPushButton("Add");
		layout->addWidget(repoAdd_, btnRow, btnCol++, 1, 1);
	}
	
	QLabel* lblName = new QLabel("Name");
	layout->addWidget(lblName, 0, rightCol, 1, 1);
	name_ = new QLineEdit();
	layout->addWidget(name_, 0, rightCol+1, 1, 2);
	
	QLabel* lblPath = new QLabel("Path");
	layout->addWidget(lblPath, 1, rightCol, 1, 1);
	path_ = new QLineEdit();
	layout->addWidget(path_, 1, rightCol+1, 1, 1);
	pathButton_ = new QPushButton("...");
	layout->addWidget(pathButton_, 1, rightCol+2, 1, 1);
	
	QLabel* lblRemotes = new QLabel("Remotes");
	layout->addWidget(lblRemotes, 2, rightCol, 1, rightSize);
	
	remotes_ = new QTreeView;
	remotes_->setModel(&remotesModel_);
	layout->addWidget(remotes_, 3, rightCol, 2, rightSize);
	
	auto btnLayout = new QHBoxLayout;
	cancel_ = new QPushButton("Cancel");
	btnLayout->addWidget(cancel_);
	commit_ = new QPushButton("Commit");
	btnLayout->addWidget(commit_);
	auto btnFrame = new QFrame;
	btnFrame->setLayout(btnLayout);
	layout->addWidget(btnFrame, 5, 0, 1, leftSize+rightSize);
	
	setLayout(layout);
	setWindowTitle("Repositories");
	
	connect(repositories_, &QTreeView::clicked, this, &UIRepositories::onSelecting);
	connect(cancel_, &QPushButton::clicked, this, &UIRepositories::onCancel);
	connect(commit_, &QPushButton::clicked, this, &UIRepositories::onCommit);
	connect(repoAdd_, &QPushButton::clicked, this, &UIRepositories::onAddRepo);
	
	repoAddUI_ = new UIRepoAdd(true);
	repoAddUI_->initialise();
	remoteAddUI_ = new UIRepoAdd(false);
	remoteAddUI_->initialise();
}

void UIRepositories::onAddRepo()
{
	auto accepted = repoAddUI_->exec();
	if (accepted == QDialog::Accepted)
	{
		auto& manager = RepositoryManager::instance();
		if (manager.add(repoAddUI_->name(), repoAddUI_->path()))
			updateUI();
	}
}

void UIRepositories::onCancel()
{
	// Reload repositories
	RepositoryManager::instance().load();
	reject();
}

void UIRepositories::onCommit()
{
	// Save to disk, reload
	RepositoryManager::instance().save();
	RepositoryManager::instance().load();
	accept();
}

void UIRepositories::onSelecting(const QModelIndex& index)
{
	selected_ = index.row();
	
	auto& manager = RepositoryManager::instance();
	if (selected_ < manager.size())
	{
		auto& repo = manager.at(selected_);
		name_->setText(repo.name());
		name_->setReadOnly(false);
		path_->setText(repo.root().absolutePath());
		path_->setReadOnly(false);
	}
	else
	{
		// Clean-up
		name_->setText("");
		name_->setReadOnly(true);
		path_->setText("");
		path_->setReadOnly(true);
	}
}

void UIRepositories::onShow()
{
	show();
}

void UIRepositories::updateUI()
{
	//repositoriesModel_.reload();
}

////////////////////////////////////////////////////////////////
/*
RepositoriesModel::RepositoriesModel()
: manager_(RepositoryManager::instance())
{
}

void RepositoriesModel::reload()
{
	beginResetModel();
	endResetModel();
}

QVariant RepositoriesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
		
	if (orientation != Qt::Horizontal)
		return QVariant();
		
	switch (section)
	{
		case 0:
			return QVariant("Name");
		default:
			return QVariant();
	}
}

QVariant RepositoriesModel::data(const QModelIndex &index, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
		
	size_t row = index.row();
	
	if (row >= manager_.size())
		return QVariant();
	
	auto& repo = manager_.at(row);
	return QVariant(repo.name());
}

QModelIndex RepositoriesModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
		
	return createIndex(row, column, nullptr);
}

QModelIndex RepositoriesModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int RepositoriesModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

int RepositoriesModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;
		
	return manager_.size();
}
*/
////////////////////////////////////////////////////////////////

RemotesModel::RemotesModel()
: manager_(RepositoryManager::instance())
{
}

QVariant RemotesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
		
	if (orientation != Qt::Horizontal)
		return QVariant();
		
	switch (section)
	{
		case 0:
			return QVariant("Name");
		case 1:
			return QVariant("URL");
		default:
			return QVariant();
	}
}

QVariant RemotesModel::data(const QModelIndex &index, int role) const
{
	//size_t row = index.row();
	
	//if (row >= files_.size())
		return QVariant();
}

QModelIndex RemotesModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
		
	return createIndex(row, column, nullptr);
}

QModelIndex RemotesModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int RemotesModel::columnCount(const QModelIndex& parent) const
{
	return 2;
}

int RemotesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || manager_.empty())
		return 0;
		
	auto& repo = manager_.active();
	return repo.remotes().size();
}

}
