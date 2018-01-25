#pragma once

#include <QFileInfo>

#include "context.h"

namespace gitigor {

class Diff {
  public:
    Diff(const QFileInfo& left, const QFileInfo& right);

    void startContext(size_t startLineLeft, size_t countLeft,
                      size_t startLineRight, size_t countRight,
                      const QString& context);
    void pushLine(const QString& line, const QString& newLine);
    void pushNewLine(const QString& line, const QString& newLine);
    void pushDeletedLine(const QString& line, const QString& newLine);

    std::pair<QFileInfo, QFileInfo> fileInfo() const {
        return std::make_pair(left_, right_);
    }
    DiffContext& currentContext() { return contexts_.back(); }

    // TODO: CONST !
    std::vector<DiffContext> contexts() const { return contexts_; }

  private:
    QFileInfo left_;
    QFileInfo right_;

    std::vector<DiffContext> contexts_;
};

} // namespace gitigor
