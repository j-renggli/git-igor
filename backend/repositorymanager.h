#pragma once

#include <memory>

#include "repository.h"

namespace gitkit {
	
class RepositoryManager {
	public:
		static RepositoryManager& instance();
		virtual ~RepositoryManager() {}
		
		bool empty() const { return repositories_.empty(); }
		size_t size() const { return repositories_.size(); }
		
		Repository& active() { return *repositories_.at(active_); }
		Repository& at(size_t index) { return *repositories_.at(index); } // TODO: UUID !?
		
		bool initialise(const QFileInfo& storagePath);
		
		bool load();
		bool save();
	
	private:
		RepositoryManager();
		RepositoryManager(const RepositoryManager& rhs);
		static RepositoryManager s_repositories;
		
		size_t active_;
		std::vector<std::unique_ptr<Repository> > repositories_;
		
		QFileInfo storagePath_;
};
	
}