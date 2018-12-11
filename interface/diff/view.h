#pragma once

#include <QtCore/QObject>

#include <QtWebEngineWidgets/QWebEngineView>

#include <backend/diff/diff.h>

#include "interface_Export.h"

class QWebChannel;
class QWebEngineProfile;

namespace gitigor {

class DiffIO;

class interface_EXPORT DiffView : public QWebEngineView {
    Q_OBJECT

  public:
    DiffView();
    virtual ~DiffView();

    bool initialise();

  signals:
    void update();

  public slots:
    void onShowDiff(const Diff& diff, bool staging);

  private:
    QWebEngineProfile* profile_;
    QWebChannel* channel_;
    DiffIO* io_{nullptr};
};
}
