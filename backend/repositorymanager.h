#pragma once

#include <memory>

#include "repository.h"

namespace gitkit {
	
class RepositoryManager {
	public:
		static RepositoryManager& instance();
		virtual ~RepositoryManager() {}
		
		bool empty() const { return repositories_.empty(); }
		
		Repository& active() { return *repositories_.at(active_); }
		
		bool initialise();
	
	private:
		RepositoryManager();
		RepositoryManager(const RepositoryManager& rhs);
		static RepositoryManager s_repositories;
		
		size_t active_;
		std::vector<std::unique_ptr<Repository> > repositories_;
};
	
}