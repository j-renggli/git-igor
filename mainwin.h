#pragma once

#include <QtCore/QFileInfo>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>

namespace gitigor {

class UIProgress;
class UIRepositories;
class UIStaging;
class UIDiffView;

class MainWin : public QMainWindow
{
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
    UIDiffView* view_;
    UIStaging* staging_;
    UIRepositories* repositories_;
};

}
