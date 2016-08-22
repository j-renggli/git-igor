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

#include <backend/actions.h>
#include <backend/backend.h>
#include <backend/repositorymanager.h>
#include <interface/view.h>
#include <interface/staging.h>

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
	/*
  for (auto it = mActions_.begin(); it != mActions_.end(); ++it)
    delete it->second;

  delete pHistory_;
  delete pCommand_;
  delete pOutput_;
  delete pToolbar_;
	*/
}

bool MainWin::initialise()
{
	if (!Backend::instance().initialise())
		return false;
		
	if (!RepositoryManager::instance().initialise())
		return false;
		
	Preferences::instance().generateCSS();
		
  pMainView_ = new QTabWidget;
  setCentralWidget(pMainView_);

  auto tab_layout = new QVBoxLayout;
  pMainView_->setLayout(tab_layout);
	
	view_ = new UIView;
	view_->doConnect();
	
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
	bool ok = Actions::initialise(this);
	if (!ok)
		return false;
		
	if (auto action = Actions::getAction(Actions::aFileQuit))
	{
		connect(action, SIGNAL(triggered()), this, SLOT(close()));
	}
	
	Backend& backend = Backend::instance();
	if (auto action = Actions::getAction(Actions::aFileRefresh))
	{
		connect(action, SIGNAL(triggered()), &backend, SLOT(onRefresh()));
	}

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
