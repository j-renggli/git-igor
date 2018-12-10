#pragma once

#include <QtCore/QObject>

class QWebEnginePage;

namespace gitigor {

class UIHistoryIO : public QObject {
    Q_OBJECT

  public:
    UIHistoryIO(QObject* parent, QWebEnginePage* page);

  public slots:
    //    void onLoaded(bool ok);

  private:
    QWebEnginePage* page_;
};
}
