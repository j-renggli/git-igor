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
    DiffLine(LineType type, const QString& line)
        : type_(type)
        , line_(line)
    {}

    const QString& line() const { return line_; }
    const LineType type() const { return type_; }

private:
    LineType type_;
    QString line_;
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

    void push(DiffLine::LineType type, const QString& line) { lines_.push_back(DiffLine(type, line)); }

    std::vector<DiffLine> linear() const { return lines_; }

    size_t startLineOld() const { return startOld_; }
    size_t startLineNew() const { return startNew_; }

    void setNoNewline(bool sideNew);

private:
    size_t startOld_;
    size_t expectedOld_;
    size_t startNew_;
    size_t expectedNew_;
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
