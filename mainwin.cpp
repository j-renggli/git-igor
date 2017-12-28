#include "mainwin.h"

#include <cstdio>
#include <iostream>
//#include <fcntl.h>
//#include <fstream>

#ifdef __linux__
#include <unistd.h>
#include <sys/wait.h>
#endif

#include <QtGui/QKeyEvent>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>

#include <backend/actions.h>
#include <backend/backend.h>
#include <backend/repositorymanager.h>

#include <interface/progress.h>
#include <interface/repositories.h>
#include <interface/staging.h>
#include <interface/diff_view.h>

#include <preferences/preferences.h>

namespace gitigor {

////////////////////////////////////////////////////////////////

MainWin::MainWin()
{
  setWindowTitle("Git-Igor");
}

////////////////////////////////////////////////////////////////

MainWin::~MainWin()
{
}

////////////////////////////////////////////////////////////////

bool MainWin::initialise()
{
	auto& backend = Backend::instance();
    if (!backend.initialise(this))
        return false;

    Preferences::instance().generateCSS();

    pMainView_ = new QTabWidget;
    setCentralWidget(pMainView_);

    auto tab_layout = new QVBoxLayout;
    pMainView_->setLayout(tab_layout);

    view_ = new UIDiffView;
    view_->doConnect();

    progress_ = new UIProgress;
    progress_->initialise();

    repositories_ = new UIRepositories;
    repositories_->initialise();

    staging_ = new UIStaging;
    staging_->initialise();
    addDockWidget(Qt::LeftDockWidgetArea, staging_);

    tab_layout->addWidget(view_);

    createActions();
    updateMenu();
    updateToolbar();

    connect(staging_, SIGNAL(onShowDiff(const std::vector<Diff>&)), view_, SLOT(onShowDiff(const std::vector<Diff>&)));

    Actions::getAction(Actions::aFileRefresh)->trigger();

    return true;
}

////////////////////////////////////////////////////////////////

bool MainWin::createActions()
{
	{
		auto action = Actions::getAction(Actions::aFileQuit);
		connect(action, &QAction::triggered, this, &MainWin::close);
	}
	
	{
		auto action = Actions::getAction(Actions::aRepoEdit);
		connect(action, &QAction::triggered, repositories_, &UIRepositories::onShow);
	}
	/*
	Backend& backend = Backend::instance();
	if (auto action = Actions::getAction(Actions::aFileRefresh))
	{
		connect(action, SIGNAL(triggered()), &backend, SLOT(onRefresh()));
	}*/

  return true;
}

////////////////////////////////////////////////////////////////

QFileInfo MainWin::style() const
{
	return Backend::instance().style();
}

////////////////////////////////////////////////////////////////

bool MainWin::updateMenu()
{
	{
		auto file = menuBar()->addMenu("&File");
		file->addAction(Actions::getAction(Actions::aFileQuit));
		file->addAction(Actions::getAction(Actions::aFileRefresh));
	}
	
	{
		auto remote = menuBar()->addMenu("&Remote");
		remote->addAction(Actions::getAction(Actions::aGitFetch));
		remote->addAction(Actions::getAction(Actions::aGitPull));
		remote->addAction(Actions::getAction(Actions::aGitPush));
	}
	
	{
		auto repos = menuBar()->addMenu("&Repositories");
		repos->addAction(Actions::getAction(Actions::aRepoEdit));
	}

  return true;
}

////////////////////////////////////////////////////////////////

bool MainWin::updateToolbar()
{
  pToolbar_ = addToolBar("File");
	pToolbar_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	pToolbar_->setIconSize(QSize(32, 32));
	
  pToolbar_->addAction(Actions::getAction(Actions::aFileQuit));
	pToolbar_->addAction(Actions::getAction(Actions::aFileRefresh));
	
	pToolbar_->addSeparator();
  pToolbar_->addAction(Actions::getAction(Actions::aGitFetch));
  pToolbar_->addAction(Actions::getAction(Actions::aGitPull));
  pToolbar_->addAction(Actions::getAction(Actions::aGitPush));

  return true;
}

////////////////////////////////////////////////////////////////

}
