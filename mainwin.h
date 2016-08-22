#pragma once

#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
//#include <QtWebKitWidgets/QWebEngineView>

//#include "commandline.h"

//#include "interface/view.h"

namespace gitkit {

class UIStaging;
class UIView;

class MainWin : public QMainWindow
{
  Q_OBJECT

public:
  MainWin();
  ~MainWin();

	bool initialise();
	
private:
  bool createActions();

  bool updateMenu();
  bool updateToolbar();

private:
  QToolBar* pToolbar_;

  QTabWidget* pMainView_;

	UIView* view_;
	UIStaging* staging_;
};

}
