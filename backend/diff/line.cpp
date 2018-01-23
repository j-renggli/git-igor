#include "line.h"

namespace gitigor {

DiffLine DiffLine::asNormalLine(size_t line) const {
    return DiffLine(DiffLine::Normal, text_, newLine_, line);
}

} // namespace gitigor
