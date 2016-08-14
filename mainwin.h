#pragma once

#include <memory>

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

private:
  bool createActions();

  bool updateMenu();
  bool updateToolbar();

//  bool eventFilter(QObject* obj, QEvent* event) override;

//private slots:
  //void onRunCommand();

private:
  std::unique_ptr<QToolBar> pToolbar_;

  /// Actions
  std::map<int, std::shared_ptr<QAction> > mActions_;

  QTabWidget* pMainView_;

	UIView* view_;
	UIStaging* staging_;
};

}
