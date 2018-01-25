#pragma once

#include <QString>

namespace gitigor {

class DiffLine {
  public:
    enum LineType { Normal, Inserted, Deleted };

  public:
    DiffLine(LineType type, const QString& text, const QString& newLine,
             size_t lineNum, size_t lineAlt = -1)
        : type_(type), line_(lineNum), lineAlt_(lineAlt), text_(text),
          newLine_(newLine) {}

    size_t line() const { return line_; }
    const QString& text() const { return text_; }
    const LineType type() const { return type_; }
    QString newLine() const { return newLine_; }

    bool missingNewLine() const { return newLine_.isEmpty(); }
    void removeNewLine() { newLine_.clear(); }

    DiffLine asNormalLine(size_t line) const;

  private:
    const LineType type_;
    const size_t line_;
    const size_t lineAlt_;
    const QString text_;
    QString newLine_;
};

} // namespace gitigor
