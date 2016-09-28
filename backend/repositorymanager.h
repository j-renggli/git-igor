#pragma once

#include <memory>

#include "repository.h"

#include <QtCore/QAbstractItemModel>

namespace gitkit {
	
class RepositoryManager : public QAbstractItemModel {
	public:
		static RepositoryManager& instance();
		virtual ~RepositoryManager() {}
		
		bool empty() const { return repositories_.empty(); }
		size_t size() const { return repositories_.size(); }
		
		Repository& active() { return *repositories_.at(active_); }
		Repository& at(size_t index) { return *repositories_.at(index); } // TODO: UUID !?
		
		bool add(const QString& name, const QDir& root);
		
		bool initialise(const QFileInfo& storagePath);
		//void reload();
		
		bool load();
		bool save();
	
	protected:
		QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
		QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
		int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
		int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
	
	private:
		RepositoryManager();
		RepositoryManager(const RepositoryManager& rhs);
		static RepositoryManager s_repositories;
		
		size_t active_;
		std::vector<std::unique_ptr<Repository> > repositories_;
		
		QFileInfo storagePath_;
};
	
}