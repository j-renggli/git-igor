#pragma once

#include <vector>

#include <QString>

#include "line.h"

namespace gitigor {

class DiffContext {
  public:
    DiffContext(size_t startOld, size_t countOld, size_t startNew,
                size_t countNew, const QString& oldFile, const QString& newFile,
                const QString& context)
        : startOld_(startOld), expectedOld_(countOld), startNew_(startNew),
          expectedNew_(countNew), fileOld_(oldFile), fileNew_(newFile),
          context_(context) {}

    void push(DiffLine::LineType type, const QString& text,
              const QString& newLine);

    QString context() const { return context_; }

    QString toPatch() const;

    DiffContext selectLines(size_t begin, size_t end) const;

    std::vector<DiffLine> lines() const { return lines_; }

    size_t startLineOld() const { return startOld_; }
    size_t startLineNew() const { return startNew_; }

    void setNoNewline(bool sideNew);

  private:
    size_t startOld_;
    size_t expectedOld_;
    size_t countOld_{0};
    size_t startNew_;
    size_t expectedNew_;
    size_t countNew_{0};

    const QString fileOld_;
    const QString fileNew_;
    const QString context_;

    std::vector<DiffLine> lines_;
    size_t lastNew_{0};
    size_t lastOld_{0};
};

} // namespace gitigor
