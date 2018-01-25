#pragma once

#include <QDir>
#include <QObject>
#include <QThread>

#include "../includes.h"

namespace gitigor {

class LogWorker : public QThread {
    Q_OBJECT

  public:
    LogWorker(const QDir& root, const QString& head);

    void run() override;

  signals:
    void onItem(const QJsonObject& item);

  private:
    void onLineRead(const QString& line);
    void sendItem();

  private:
    const QDir root_;
    const QString head_;
    QJsonObject* current_{nullptr};
};

class backend_EXPORT GitLogger : public QObject {
    Q_OBJECT

  public:
    GitLogger(const QDir& root);
    ~GitLogger();

  public:
    bool start(const QString& head = "HEAD");
    void stop();

  signals:
    void onItem(const QJsonObject& item);
    void onFinished(int exitCode);

  private:
    bool isRunning() const;

  private:
    const QDir root_;
    QThread* workerThread_{nullptr};
};

} // namespace gitigor
