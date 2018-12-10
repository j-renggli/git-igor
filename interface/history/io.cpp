#include "io.h"

#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineScript>

namespace gitigor
{

UIHistoryIO::UIHistoryIO(QObject* parent, QWebEnginePage* page) : QObject(parent), page_(page)
{
    Q_ASSERT(page_);
}

} // namespace gitigor
