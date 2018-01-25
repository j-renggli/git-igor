#pragma once

#include <QtCore/QObject>

#include <backend/diff/diff.h>

class QWebEnginePage;

namespace gitigor {

class DiffIO : public QObject {
    Q_OBJECT

  public:
    DiffIO(QObject* parent, QWebEnginePage* page);
    ~DiffIO();

  public:
    void setActiveDiff(const Diff& diff);

  public slots:
    void onStageHunk(int hunk);
    bool onStageLines(int hunk, int begin, int end);

  private:
    QWebEnginePage* page_;
    // TODO. std::optional requires gcc 7+
    Diff* currentDiff_{nullptr};
};

} // namespace gitigor
