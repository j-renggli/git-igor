#pragma once

#include <QtCore/QObject>

#include <QtWebEngineWidgets/QWebEngineView>

#include <backend/diff.h>

class QWebChannel;
class QWebEngineProfile;

namespace gitkit {

class UIDiffIO;

class UIDiffView : public QWebEngineView
{
    Q_OBJECT

public:
    UIDiffView();
    virtual ~UIDiffView();

    void doConnect();

signals:
    void update();

private slots:
    void onShowDiff(const std::vector<Diff>& diff);

private:
    QWebEngineProfile* profile_;
    QWebChannel* channel_;
    UIDiffIO* diffio_{nullptr};

};

}
