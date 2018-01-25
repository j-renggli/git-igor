#include "history_view.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QtCore/QDebug>
#include <QtWebChannel/QWebChannel>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWebEngineWidgets/QWebEngineScriptCollection>

#include <backend/backend.h>
#include <backend/git/logger.h>
#include <backend/git/process.h>
#include <backend/repository.h>

#include "history_io.h"

namespace gitigor {

UIHistoryView::UIHistoryView() {}

UIHistoryView::~UIHistoryView() {}

bool UIHistoryView::initialise() {
    profile_ = new QWebEngineProfile("Git Log History", this);
    channel_ = new QWebChannel(this);

    Backend& backend = Backend::instance();
    QDir htmlRoot = backend.htmlFolder();

    {
        // Set-up web channel
        QFile qwebchannel(":/qtwebchannel/qwebchannel.js");
        if (!qwebchannel.open(QIODevice::ReadOnly))
            throw std::runtime_error("Missing web channel file");

        QWebEngineScript script;
        script.setName("qwebchannel.js");
        script.setSourceCode(qwebchannel.readAll());
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setInjectionPoint(QWebEngineScript::DocumentCreation);
        profile_->scripts()->insert(script);
    }

    {
        // Angular js
        QFile angular(htmlRoot.absoluteFilePath("angular.min.js"));
        if (!angular.open(QIODevice::ReadOnly))
            throw std::runtime_error("Missing Angular file");

        QWebEngineScript script;
        script.setName("angular.min.js");
        script.setSourceCode(angular.readAll());
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setInjectionPoint(QWebEngineScript::DocumentCreation);
        profile_->scripts()->insert(script);
    }

    {
        // Main code
        QFile history(htmlRoot.absoluteFilePath("history.js"));
        if (!history.open(QIODevice::ReadOnly))
            throw std::runtime_error("Missing history script");

        QWebEngineScript script;
        script.setName("history.js");
        script.setSourceCode(history.readAll());
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setInjectionPoint(QWebEngineScript::DocumentCreation);
        profile_->scripts()->insert(script);
    }

    {
        QWebEngineScript script;
        script.setName("qtio.js");
        script.setSourceCode("new QWebChannel(window.qt.webChannelTransport, "
                             "function(channel) {\n"
                             "  window.qtio = channel.objects.qtio;\n"
                             "  window.qtjs = angular;\n"
                             "  window.addCommit = function() {\n"
                             "    alert(this);\n"
                             "  }\n"
                             /* "  window.wrappers = {\n"
                              "    stageHunk: function(index) {\n"
                              "      var name='diff_'+index;\n"
                              "
     document.getElementById(name).classList.add('hidden');\n"
                              "      window.qtio.onStageHunk(index);\n"
                              "    }\n"
                              "  }\n"*/
                             "});");
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setInjectionPoint(QWebEngineScript::DocumentReady);
        script.setRunsOnSubFrames(false);
        profile_->scripts()->insert(script);
        setPage(new QWebEnginePage(profile_, this));
    }

    Q_ASSERT(!historyio_);
    historyio_ = new UIHistoryIO(this, page());
    channel_->registerObject(QStringLiteral("qtio"), historyio_);
    page()->setWebChannel(channel_);
    show();

    QFile historyHtml(htmlRoot.absoluteFilePath("history.html"));
    if (!historyHtml.open(QIODevice::ReadOnly))
        throw std::runtime_error("Missing history html file");

    setHtml(historyHtml.readAll());

    // TODO: some better place
    connect(page(), &QWebEnginePage::loadFinished, [&](bool ok) {
        if (!ok)
            return;

        if (backend.empty())
            return;

        showActive(backend.currentRepo());
    });

    return true;
}

void UIHistoryView::nextItem(const QJsonObject& item) {
    Q_ASSERT(historyio_);
    QString json = QJsonDocument(item).toJson(QJsonDocument::Compact);
    // qDebug() << json;
    QString js =
        QString("var scope = "
                "angular.element(document.getElementById('history')).scope();\n"
                "scope.addCommit(%1);")
            .arg(json);
    page()->runJavaScript(js);
}

void UIHistoryView::showActive(const Repository& repository) {
    if (logger_) {
        logger_->stop();
        disconnect(cnxNextItem_);
    }

    // QDir repoPath_ = repository.root();
    // QStringList historyHeads_;
    // historyHeads_ << repository.activeBranch();

    logger_ = repository.prepareGitLogger();
    if (!logger_)
        return;

    cnxNextItem_ = connect(logger_.get(), &GitLogger::onItem, this,
                           &UIHistoryView::nextItem);

    logger_->start();
}

void UIHistoryView::update() {}

} // namespace gitigor
