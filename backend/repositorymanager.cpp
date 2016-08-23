#include "repositorymanager.h"

#include <iostream>

#include <gkassert.h>

namespace gitkit {

RepositoryManager RepositoryManager::s_repositories;
	
RepositoryManager::RepositoryManager()
: active_(-1)
{
}

RepositoryManager::RepositoryManager(const RepositoryManager& rhs)
{
	ASSERT(false);
}

RepositoryManager& RepositoryManager::instance()
{
	return s_repositories;
}

bool RepositoryManager::initialise()
{
	repositories_.push_back(std::unique_ptr<Repository>(new Repository(QString("DogFood"), QDir("."))));
	repositories_.back()->initialise();
	active_ = 0;
	
	return true;
}
	
}