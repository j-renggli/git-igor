#pragma once

#include <QtCore/QObject>

#include <backend/diff.h>

class QWebEnginePage;

namespace gitkit {

class UIDiffIO : public QObject
{
    Q_OBJECT

public:
    UIDiffIO(QObject* parent, QWebEnginePage* page);

public:
    void setActiveDiff(const std::vector<Diff>& diff);

public slots:
    void onStageHunk(int hunk);

private:
    QWebEnginePage* page_;
    std::vector<Diff> currentDiff_;
};

}
