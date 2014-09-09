#pragma once

#include <QtGui/QAction>
#include <QtGui/QMainWindow>
#include <QtGui/QTableView>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>

#include "commandline.h"

namespace ccc {

class MainWin : public QMainWindow
{
  Q_OBJECT

public:
  MainWin();
  ~MainWin();

private:
  bool createActions();

  bool updateMenu();
  bool updateToolbar();

//  bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
  void onRunCommand();

private:
  QToolBar* pToolbar_;

  /// Actions
  std::map<int, QAction*> mActions_;

  QTabWidget* pMainView_;

  QTableView* pHistory_;
  CommandLine* pCommand_;
  QTextEdit* pOutput_;

};

}
