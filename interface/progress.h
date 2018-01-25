#pragma once

#include <memory>

#include <QtWidgets/QDialog>

#include "includes.h"

class QLineEdit;
class QProgressBar;
class QTextEdit;

namespace gitigor {

class interface_EXPORT UIProgress : public QDialog {
    Q_OBJECT

  public:
    UIProgress();
    virtual ~UIProgress();

    void initialise();

  public slots:
    void onShow();
    void onSetCommand(const QString& command);
    void onOutput(const QString& output);
    /*void onError(const QString& error);
    */
    void onWorkDone();

  private:
    QLineEdit* command_;
    QTextEdit* output_;
    QProgressBar* progress_;
};
}
