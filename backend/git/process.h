#pragma once

#include <QDir>
#include <QString>

namespace gitigor {

class GitProcess {
  public:
    enum eCommand {
        Add,
        Apply,
        Commit,
        Delete,
        Diff,
        Log,
        Push,
        Reset,
        RevParse,
        Show,
        Status,
    };

  public:
    GitProcess(const QDir& root);

    // Start a process and wait for completion, optionally reading standard
    // output / error
    bool run(eCommand command, QStringList args, bool readOut, bool readErr);
    bool runWithInput(eCommand command, QStringList args, QByteArray input,
                      bool readOut, bool readErr);

    QString err() const { return stdErr_; }

    QStringList linesOut() const;
    QString out() const { return stdOut_; }

  private:
    QStringList prepareArgs(eCommand command, QStringList args);

  private:
    QString root_;

    QString stdErr_;
    QString stdOut_;

    static const QRegularExpression s_rxLineEnd;
};

} // namespace gitigor
