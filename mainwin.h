#pragma once

#include <QtCore/QFileInfo>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>

namespace gitigor {

class UIProgress;
class UIRepositories;
class UIStaging;
class DiffView;
class UIHistoryView;

class MainWin : public QMainWindow {
    Q_OBJECT

  public:
    MainWin();
    ~MainWin();

    bool initialise();

    QFileInfo style() const;

  private:
    bool createActions();

    bool updateMenu();
    bool updateToolbar();

  private:
    QToolBar* pToolbar_;

    QTabWidget* pMainView_;

    UIProgress* progress_;
    DiffView* view_;
    UIStaging* staging_;
    UIHistoryView* history_;
    UIRepositories* repositories_;
};
}
