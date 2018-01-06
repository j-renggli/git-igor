#pragma once

#include <QDir>
#include <QObject>
#include <QThread>

namespace gitigor {

struct LogItem {
    LogItem() = default;
    LogItem(const LogItem& item) = default;

    QString id;
    QString summary;
};

class LogWorker : public QThread {
    Q_OBJECT

  public:
    LogWorker(const QDir& root, const QString& head);

    void run() override;

  signals:
    void onItem(const gitigor::LogItem& item);

  private:
    void onLineRead(const QString& line);
    void sendItem();

  private:
    const QDir root_;
    const QString head_;
    LogItem* current_{nullptr};
};

class GitLogger : public QObject {
    Q_OBJECT

  public:
    GitLogger(const QDir& root);
    ~GitLogger();

  public:
    bool start(const QString& head = "HEAD");
    void stop();

  signals:
    void onItem(const gitigor::LogItem& item);
    void onFinished(int exitCode);

  private:
    bool isRunning() const;

  private:
    const QDir root_;
    QThread* workerThread_{nullptr};
};

} // namespace gitigor

Q_DECLARE_METATYPE(gitigor::LogItem)
