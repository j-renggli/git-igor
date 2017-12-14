#pragma once

#include <QDir>
#include <QString>

namespace gitkit {

class GitProcess {
public:
    enum eCommand {
        Add,
        Commit,
        Delete,
        Diff,
        Push,
        Reset,
        RevParse,
        Show,
        Status,
    };

public:
    GitProcess(const QDir& root);

    bool run(eCommand command, QStringList args, bool readOut, bool readErr);

    QStringList linesOut() const;
    QString out() const { return stdOut_; }

private:
    QString root_;

    QString stdOut_;

    static const QRegExp s_rxLineEnd;
};

}
