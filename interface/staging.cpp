#include "staging.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>

#include <backend/backend.h>
#include <backend/repository.h>

#include <iostream>

namespace gitkit {

UIStaging::UIStaging()
: indexModel_(true)
, workTreeModel_(false)
{
}

UIStaging::~UIStaging()
{
}

void UIStaging::initialise()
{
	auto layout = new QVBoxLayout;
	index_ = new QTreeView;
	index_->setModel(&indexModel_);
	workTree_ = new QTreeView;
	workTree_->setModel(&workTreeModel_);
	
	message_ = new QTextEdit;
	message_->setAcceptRichText(false);
	message_->setPlaceholderText("Enter a commit message here...");
	commit_ = new QPushButton("Commit");
	pullCommitPush_ = new QPushButton("Pull, Commit, Push");
	
	QLabel* lblIndex = new QLabel("Index");
	QLabel* lblWork = new QLabel("Work Tree");
	
	layout->addWidget(lblWork);
	layout->addWidget(workTree_);
	layout->addWidget(lblIndex);
	layout->addWidget(index_);
	layout->addWidget(message_);
	
	auto buttons = new QHBoxLayout;
	buttons->addWidget(commit_);
	buttons->addWidget(pullCommitPush_);
	layout->addLayout(buttons);
	
	auto frame = new QWidget;
	frame->setLayout(layout);
	setWidget(frame);
	setWindowTitle("Staging");
	show();
	
	Backend& backend = Backend::instance();
	connect(&backend, SIGNAL(onRepoUpdated()), this, SLOT(onUpdate()));
	
	connect(workTree_, SIGNAL(clicked(QModelIndex)), this, SLOT(onShowWorkTreeFile(QModelIndex)));
	connect(workTree_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onStageFile(QModelIndex)));
	connect(index_, SIGNAL(clicked(QModelIndex)), this, SLOT(onShowIndexFile(QModelIndex)));
	connect(index_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onUnstageFile(QModelIndex)));
	
	connect(commit_, SIGNAL(clicked(bool)), this, SLOT(onCommit(bool)));
}

void UIStaging::onCommit(bool checked)
{
	Backend& backend = Backend::instance();
	const Repository& repo = backend.currentRepo();
	repo.commit(message_->toPlainText());
	
	message_->setPlainText("");
	//message_->setPlaceholderText("Enter a commit message here...");
	
	backend.onRefresh();
}

void UIStaging::onUpdate()
{
	Backend& backend = Backend::instance();
	const Repository& repo = backend.currentRepo();
	
	auto changes = repo.inStage();
	indexModel_.clear();
	workTreeModel_.clear();
	for (const auto& change : changes)
	{
		if (change.status(true) != FileStatus::NORMAL)
		{
			indexModel_.addFile(change);
		}
		
		if (change.status(false) != FileStatus::NORMAL)
		{
			workTreeModel_.addFile(change);
		}
	}
	
	indexModel_.update();
	workTreeModel_.update();
}

void UIStaging::onStageFile(const QModelIndex& index)
{
	Backend& backend = Backend::instance();
	const Repository& repo = backend.currentRepo();
	repo.stage(workTreeModel_.getFile(index.row()));
	backend.onRefresh();
}

void UIStaging::onShowWorkTreeFile(const QModelIndex& index)
{
	Backend& backend = Backend::instance();
	const Repository& repo = backend.currentRepo();
	auto diffs = repo.diff(workTreeModel_.getFile(index.row()), false);
	onShowDiff(diffs);
}

void UIStaging::onUnstageFile(const QModelIndex& index)
{
	Backend& backend = Backend::instance();
	const Repository& repo = backend.currentRepo();
	repo.unstage(indexModel_.getFile(index.row()));
	backend.onRefresh();
}

void UIStaging::onShowIndexFile(const QModelIndex& index)
{
	Backend& backend = Backend::instance();
	const Repository& repo = backend.currentRepo();
	auto diffs = repo.diff(indexModel_.getFile(index.row()), true);
	onShowDiff(diffs);
}

////////////////////////////////////////////////////////////////

StageModel::StageModel(bool index)
: index_(index)
{
}

void StageModel::clear()
{
	beginResetModel();
	files_.clear();
}

void StageModel::addFile(const FileStatus& file)
{
	files_.push_back(file);
}

FileStatus StageModel::getFile(size_t index) const
{
	return files_.at(index);
}

void StageModel::update()
{
	std::sort(files_.begin(), files_.end());
	endResetModel();
}

QVariant StageModel::data(const QModelIndex &index, int role) const
{
	size_t row = index.row();
	
	if (row >= files_.size())
		return QVariant();
		
	switch (role)
	{
		case Qt::DisplayRole:
			return QVariant(files_.at(row).path().filePath());
		case Qt::DecorationRole: {
			switch (files_.at(row).status(index_))
			{
			case FileStatus::NORMAL:
				//ASSERT(false);
				break;
			case FileStatus::ADDED:
				return QVariant(QColor(0, 255, 0));
			case FileStatus::MODIFIED:
				return QVariant(QColor(0, 0, 255));
			case FileStatus::DELETED:
				return QVariant(QColor(255, 0, 0));
			case FileStatus::RENAMED:
				return QVariant(QColor(255, 0, 255));
			default:
				break;
			}
			break;
		}
		default:
			return QVariant();
	}
	
	return QVariant();
}

Qt::ItemFlags StageModel::flags(const QModelIndex &index) const
{
	return QAbstractItemModel::flags(index);
}

QVariant StageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
		
	if (orientation != Qt::Horizontal)
		return QVariant();
		
	size_t row = section;
	if (row >= 1)
		return QVariant();
		
	return QVariant("Path");
}

QModelIndex StageModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
		
	return createIndex(row, column, nullptr);//&files_.at(row));
}

QModelIndex StageModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}

int StageModel::columnCount(const QModelIndex& parent) const
{
//	if (parent.isValid())
//		return 0;
		
	return 1;
}

int StageModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;
		
	return files_.size();
}
	
}
