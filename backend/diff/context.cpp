#include "context.h"

namespace gitigor {

void DiffContext::push(DiffLine::LineType type, const QString& text,
                       const QString& newLine) {
    size_t line = 0;
    size_t lineAlt(-1);
    switch (type) {
    case DiffLine::Normal:
        line = startOld_ + countOld_++;
        lineAlt = startNew_ + countNew_++;
        lastNew_ = lines_.size();
        lastOld_ = lines_.size();
        break;
    case DiffLine::Inserted:
        line = startNew_ + countNew_++;
        lastNew_ = lines_.size();
        break;
    case DiffLine::Deleted:
        line = startOld_ + countOld_++;
        lastOld_ = lines_.size();
        break;
    }

    lines_.push_back(DiffLine(type, text, newLine, line, lineAlt));
}

void DiffContext::setNoNewline(bool sideNew) {
    if (sideNew) {
        lines_.at(lastNew_).removeNewLine();
    } else {
        lines_.at(lastOld_).removeNewLine();
    }
}

QString DiffContext::toPatch() const {
    QString patch = QString("diff --git a/%1 b/%2\n"
                            "--- a/%1\n"
                            "+++ b/%2\n"
                            "@@ -%3,%4 +%5,%6 @@\n")
                        .arg(fileOld_)
                        .arg(fileNew_)
                        .arg(startOld_)
                        .arg(countOld_)
                        .arg(startNew_)
                        .arg(countNew_);

    for (const auto& line : lines_) {
        QString type;
        switch (line.type()) {
        case DiffLine::Normal:
            type = " ";
            break;
        case DiffLine::Inserted:
            type = "+";
            break;
        case DiffLine::Deleted:
            type = "-";
            break;
        }
        auto newLine = line.newLine();
        patch += QString("%1%2%3").arg(type).arg(line.text()).arg(newLine);
        if (newLine.isEmpty())
            patch += "\n\\ No newline at end of file\n";
    }
    return patch;
}

DiffContext DiffContext::selectLines(size_t begin, size_t end) const {
    std::vector<DiffLine> selected;
    selected.reserve(lines_.size());
    const size_t startOld = startOld_;
    const size_t startNew = startNew_;
    size_t countOld = 0;
    size_t countNew = 0;
    size_t globalNr = 0;
    for (const auto& line : lines_) {
        if (globalNr >= begin && globalNr < end) {
            if (line.type() != DiffLine::Inserted)
                ++countOld;
            if (line.type() != DiffLine::Deleted)
                ++countNew;
            selected.push_back(line);
        } else if (globalNr > end + 3) {
            break;
        } else if (line.type() != DiffLine::Inserted) {
            // Add only as "old"
            selected.push_back(line.asNormalLine(startOld + countOld));
            ++countOld;
            ++countNew;
        }
        ++globalNr;
    }

    DiffContext selection(startOld, countOld, startNew, countNew, fileOld_,
                          fileNew_, context_);
    selection.countOld_ = countOld;
    selection.countNew_ = countNew;
    selection.lines_.swap(selected);
    return selection;
}

} // namespace gitigor
