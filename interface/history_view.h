#pragma once

#include <memory>

#include <QtWebEngineWidgets/QWebEngineView>

//#include <backend/repository.h>
#include <backend/git/logger.h> // Necessary due to registered LogItem...

class QWebChannel;
class QWebEngineProfile;

namespace gitigor {

class GitLogger;
class Repository;
class UIHistoryIO;

class UIHistoryView : public QWebEngineView {
    Q_OBJECT

  public:
    UIHistoryView();
    virtual ~UIHistoryView();

  public:
    bool initialise();

  private:
    void update();
    // signals:
    //    void update();

  public slots:
    void showActive(const Repository& repository);

  private slots:
    void nextItem(const gitigor::LogItem& item);

  private:
    QWebEngineProfile* profile_;
    QWebChannel* channel_;
    UIHistoryIO* historyio_{nullptr};

    // QDir repoPath_;
    // QString repositoryName_;
    // QStringList historyHeads_;

    std::unique_ptr<GitLogger> logger_;
    QMetaObject::Connection cnxNextItem_;
};

} // namespace gitigor
