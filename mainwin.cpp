#include "mainwin.h"

#include <cstdio>
#include <iostream>
//#include <fcntl.h>
//#include <fstream>
#include <unistd.h>
#include <sys/wait.h>

#include <QtGui/QKeyEvent>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>

#include <gkassert.h>

#include <backend/actions.h>
#include <backend/backend.h>
#include <backend/repositorymanager.h>

#include <interface/progress.h>
#include <interface/repositories.h>
#include <interface/staging.h>
#include <interface/view.h>

#include <preferences/preferences.h>

namespace gitkit {

////////////////////////////////////////////////////////////////

MainWin::MainWin()
{
  setWindowTitle("Git-Kit");
}

////////////////////////////////////////////////////////////////

MainWin::~MainWin()
{
}

////////////////////////////////////////////////////////////////

bool MainWin::initialise()
{
	bool ok = Actions::initialise(this);
	if (!ok)
		return false;
		
	auto& backend = Backend::instance();
	if (!backend.initialise())
		return false;
		
	Preferences::instance().generateCSS();
		
  pMainView_ = new QTabWidget;
  setCentralWidget(pMainView_);

  auto tab_layout = new QVBoxLayout;
  pMainView_->setLayout(tab_layout);
	
	view_ = new UIView;
	view_->doConnect();
	
	progress_ = new UIProgress;
	progress_->initialise();
	
	repositories_ = new UIRepositories;
	repositories_->initialise();
	repositories_->onShow(true);
	
	staging_ = new UIStaging;
	staging_->initialise();
	addDockWidget(Qt::LeftDockWidgetArea, staging_);

	tab_layout->addWidget(view_);
	
  createActions();
  updateMenu();
  updateToolbar();
	
	connect(staging_, SIGNAL(onShowDiff(const std::vector<Diff>&)), view_, SLOT(onShowDiff(const std::vector<Diff>&)));
	return true;
}

////////////////////////////////////////////////////////////////

bool MainWin::createActions()
{
	if (auto action = Actions::getAction(Actions::aFileQuit))
	{
		connect(action, SIGNAL(triggered()), this, SLOT(close()));
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

//bool MainWin::eventFilter(QObject* obj, QEvent* event)
//{
//  if (obj == pCommand_ && event->type() == QEvent::KeyPress)
//  {
//    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
//    if (keyEvent->key() == Qt::Key_Tab)
//    {
//      pCommand_->tryAutocomplete();
//      return true;
//    }
//  }
//
//  return QMainWindow::eventFilter(obj, event);
//}

////////////////////////////////////////////////////////////////

bool MainWin::updateMenu()
{
  auto file = menuBar()->addMenu("&File");
  file->addAction(Actions::getAction(Actions::aFileQuit));
	file->addAction(Actions::getAction(Actions::aFileRefresh));
	
  auto remote = menuBar()->addMenu("&Remote");
  remote->addAction(Actions::getAction(Actions::aGitFetch));
  remote->addAction(Actions::getAction(Actions::aGitPull));
  remote->addAction(Actions::getAction(Actions::aGitPush));

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
