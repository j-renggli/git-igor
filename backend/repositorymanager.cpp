#include "repositorymanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

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

bool RepositoryManager::add(const QString& name, const QDir& root)
{
	repositories_.push_back(std::unique_ptr<Repository>(new Repository(name, root)));
	return repositories_.back()->initialise();
}

bool RepositoryManager::initialise(const QFileInfo& storagePath)
{
	storagePath_ = storagePath;
	
	repositories_.push_back(std::unique_ptr<Repository>(new Repository(QString("DogFood"), QDir("."))));
	repositories_.back()->initialise();
	active_ = 0;
	
	return load();
}

bool RepositoryManager::load()
{
	// Empty repos...
	repositories_.clear();
	
	QFile file(storagePath_.filePath());
	if (!file.exists())
		return true;
		
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}
	
	QJsonDocument json(QJsonDocument::fromJson(file.readAll()));
	QJsonObject main = json.object();
	int version = main["version"].toInt();
	ASSERT(version == 1);
	if (version != 1)
		return false;
	
	QJsonArray repos = main["repositories"].toArray();
	for (int i = 0; i < repos.size(); ++i) {
		QJsonObject item = repos[i].toObject();
		repositories_.push_back(std::unique_ptr<Repository>(new Repository(item["name"].toString(), QDir(item["root"].toString()))));
		repositories_.back()->initialise();
	}
	
	active_ = main["active"].toInt();
	
	return true;
}

bool RepositoryManager::save()
{
	QFile file(storagePath_.filePath());
	if (!file.open(QIODevice::WriteOnly)) {
		return false;
	}
	
	QJsonObject main;
	main["version"] = 1;
	QJsonArray repos;
	for (auto& repo : repositories_) {
		QJsonObject item;
		item["name"] = repo->name();
		item["root"] = repo->root().absolutePath();
		repos.append(item);
	}
	main["repositories"] = repos;
	main["active"] = static_cast<int>(active_);
	
	QJsonDocument json(main);
	file.write(json.toJson());
	return true;
}

}