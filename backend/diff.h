#pragma once

#include <QtCore/QDir>

namespace gitkit {

class DiffLine
{
public:
    enum LineType
    {
        Normal,
        Inserted,
        Deleted
    };

public:
    DiffLine(LineType type, const QString& text, size_t lineNum, size_t lineAlt = -1)
        : type_(type)
        , line_(lineNum)
        , lineAlt_(lineAlt)
        , text_(text)
    {}

    size_t line() const { return line_; }
    const QString& text() const { return text_; }
    const LineType type() const { return type_; }

private:
    const LineType type_;
    const size_t line_;
    const size_t lineAlt_;
    const QString text_;
};

class DiffContext
{
public:
    DiffContext(size_t startOld, size_t countOld, size_t startNew, size_t countNew, const QString& context)
        : startOld_(startOld)
        , expectedOld_(countOld)
        , startNew_(startNew)
        , expectedNew_(countNew)
        , context_(context)
    {}

    void push(DiffLine::LineType type, const QString& text);

    QString context() const { return context_; }

    std::vector<DiffLine> lines() const { return lines_; }

    bool hasNewLineOld() const { return noNewlineOld_; }
    bool hasNewLineNew() const { return noNewlineNew_; }

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
    const QString context_;

    bool noNewlineOld_{false};
    bool noNewlineNew_{false};

    std::vector<DiffLine> lines_;
};

class Diff
{
public:
    Diff(const QFileInfo& left, const QFileInfo& right);

    void startContext(size_t startLineLeft, size_t countLeft, size_t startLineRight, size_t countRight, const QString& context);
    void pushLine(const QString& line);
    void pushNewLine(const QString& line);
    void pushDeletedLine(const QString& line);

    std::pair<QFileInfo, QFileInfo> fileInfo() const { return std::make_pair(left_, right_); }
    DiffContext& currentContext() { return contexts_.back(); }

    // TODO: CONST !
    std::vector<DiffContext> contexts() const { return contexts_; }

private:
    QFileInfo left_;
    QFileInfo right_;

    std::vector<DiffContext> contexts_;
};

}
