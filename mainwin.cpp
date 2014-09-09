#include "mainwin.h"

#include <cstdio>
//#include <iostream>
//#include <fcntl.h>
//#include <fstream>
#include <unistd.h>
#include <sys/wait.h>

#include <QtGui/QKeyEvent>
#include <QtGui/QMenuBar>
#include <QtGui/QVBoxLayout>

namespace ccc {

////////////////////////////////////////////////////////////////

MainWin::MainWin()
{
  pMainView_ = new QTabWidget;
  setCentralWidget(pMainView_);

  auto tab_layout = new QVBoxLayout;
  pMainView_->setLayout(tab_layout);

  pHistory_ = new QTableView;

  pCommand_ = new CommandLine;
//  connect(pCommand_, SIGNAL(textChanged()), this, SLOT(onRunCommand()));
  connect(pCommand_, SIGNAL(returnPressed()), this, SLOT(onRunCommand()));
  pCommand_->setFocus();

  pOutput_ = new QTextEdit;
  pOutput_->setReadOnly(true);
  pOutput_->setText("Here comes the output");

  tab_layout->addWidget(pHistory_);
  tab_layout->addWidget(pCommand_);
  tab_layout->addWidget(pOutput_);

  createActions();
  updateMenu();
  updateToolbar();

  setWindowTitle("CCC");
}

////////////////////////////////////////////////////////////////

MainWin::~MainWin()
{
  for (auto it = mActions_.begin(); it != mActions_.end(); ++it)
    delete it->second;

  delete pHistory_;
  delete pCommand_;
  delete pOutput_;
  delete pToolbar_;
}

////////////////////////////////////////////////////////////////

bool MainWin::createActions()
{
  auto quit = new QAction("&Quit", this);
  quit->setShortcuts(QKeySequence::Quit);
  quit->setStatusTip("Quit the application");
  connect(quit, SIGNAL(triggered()), this, SLOT(close()));
  mActions_[0] = quit;

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

void MainWin::onRunCommand()
{
  const std::vector<std::string> arguments = pCommand_->getCommandArgs();
  for (size_t i = 0; i < arguments.size(); ++i)
    std::cout << arguments[i] << "][";
  std::cout << std::endl;

  pid_t pid = 0;
  int out_pipe[2];
  int err_pipe[2];
  pipe(out_pipe);
  pipe(err_pipe);
  pid = fork();
  if (pid == 0)
  {
    // Child process
    ::close(out_pipe[0]);
    ::close(err_pipe[0]);
    dup2(out_pipe[1], STDOUT_FILENO);
    dup2(err_pipe[1], STDERR_FILENO);

//    std::cout << "Run" << std::endl;
    execl("/bin/ls", "ls", "/home/jrenggli", /*"ls",*/ (char*)0);
    std::cout << "Done" << std::endl;
    _exit(127);
  }

  std::cout << "Parent" << std::endl;
  ::close(out_pipe[1]);
  ::close(err_pipe[1]);

  if (pid < 0)
  {
    std::cout << ":(((" << std::endl;
    ::close(out_pipe[1]);
    ::close(err_pipe[1]);
  } else {
    std::cout << "wait..." << std::endl;
    FILE* out = fdopen(out_pipe[0], "r");
    FILE* err = fdopen(err_pipe[0], "r");

    const int buf_size = 1024;
    char output[buf_size];
    while (fgets(output, buf_size, out))
    {
      std::cout << "OUT: " << output << std::endl;
    }
    std::cout << "FINOUT" << std::endl;
    while (fgets(output, buf_size, err))
    {
      std::cout << "ERR: " << output << std::endl;
    }
    std::cout << "FINERR" << std::endl;
    int status;
    waitpid(pid, &status, 0);
  }

  pOutput_->setText("");
}

////////////////////////////////////////////////////////////////

bool MainWin::updateMenu()
{
  auto file = menuBar()->addMenu("&File");
  file->addAction(mActions_[0]);

  return true;
}

////////////////////////////////////////////////////////////////

bool MainWin::updateToolbar()
{
  pToolbar_ = addToolBar("File");
  pToolbar_->addAction(mActions_[0]);

  return true;
}

////////////////////////////////////////////////////////////////

}
