#include "history_io.h"

#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineScript>

namespace gitigor {

UIHistoryIO::UIHistoryIO(QObject* parent, QWebEnginePage* page)
    : QObject(parent), page_(page) {
    Q_ASSERT(page_);
}

void UIHistoryIO::addCommit() {
    // page_->runJavaScript(test);
}

void UIHistoryIO::onLoaded(bool ok) {
    // QString test("alert(Object.keys(this));");
    QString test(
        "var scope = "
        "angular.element(document.getElementById('history')).scope();"
        "//alert(scope);\n"
        "scope.addCommit({id: '111', parents: [], summary:'qwerasdf'});");
    page_->runJavaScript(test);
}

} // namespace gitigor
