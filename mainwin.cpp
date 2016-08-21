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

#include <backend/backend.h>
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
	std::shared_ptr<QAction> quit(new QAction("&Quit", this));
  quit->setShortcuts(QKeySequence::Quit);
  quit->setStatusTip("Quit the application");
  connect(quit.get(), SIGNAL(triggered()), this, SLOT(close()));
  mActions_[0] = quit;
	
	Backend& backend = Backend::instance();
	std::shared_ptr<QAction> refresh(new QAction("Refresh", this));
	refresh->setShortcuts(QKeySequence::Refresh);
	refresh->setStatusTip("Refresh the UI");
	connect(refresh.get(), SIGNAL(triggered()), &backend, SLOT(onRefresh()));
	mActions_[1] = refresh;

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

//void MainWin::onRunCommand()
//{
//}

////////////////////////////////////////////////////////////////

bool MainWin::updateMenu()
{
  auto file = menuBar()->addMenu("&File");
  file->addAction(mActions_[0].get());
	file->addAction(mActions_[1].get());

  return true;
}

////////////////////////////////////////////////////////////////

bool MainWin::updateToolbar()
{
  pToolbar_.reset(addToolBar("File"));
  pToolbar_->addAction(mActions_[0].get());
	pToolbar_->addAction(mActions_[1].get());

  return true;
}

////////////////////////////////////////////////////////////////

}
