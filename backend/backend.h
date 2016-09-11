#pragma once

#include <QtCore/QDir>
#include <QtCore/QObject>

#include "repository.h"
#include "runner.h"

namespace gitkit {

class Backend : public QObject
{
  Q_OBJECT
	
public:
	static Backend& instance();
	virtual ~Backend() {}
	
	const Repository& currentRepo() const;// { return repositories_.at(current_repo_); }
	const ActionRunner& taskRunner() const { return runner_; }
	
	const QDir& configFolder() const { return configPath_; }
	const QDir& cssFolder() const { return cssPath_; }
	
	bool initialise();

private:
	Backend();

signals:
	void onRepoUpdated();
	
public slots:
	Q_INVOKABLE void onRefresh();
	Q_INVOKABLE void onFetch();
	Q_INVOKABLE void onPull();
	Q_INVOKABLE void onPush();
	
	void workStarted(const QString& command);
	void workEnded();
	void workOutput(const QString& output);
	
private:
	/// Path to the git executable
	QDir gitPath_;
	
	QDir configPath_;
	QDir cssPath_;
	
	/// Known repositories
	//std::vector<Repository> repositories_;
	//size_t current_repo_;
	ActionRunner runner_;
	
private:
	static Backend s_backend;
};

}
