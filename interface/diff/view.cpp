#include "view.h"

#include <QtWebChannel/QWebChannel>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWebEngineWidgets/QWebEngineScriptCollection>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <backend/backend.h>

#include "io.h"

namespace gitigor {

DiffView::DiffView() {}

DiffView::~DiffView() {}

bool DiffView::initialise() {
    profile_ = new QWebEngineProfile("Git Diff", this);
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
        QFile history(htmlRoot.absoluteFilePath("staging.js"));
        if (!history.open(QIODevice::ReadOnly))
            throw std::runtime_error("Missing staging script");

        QWebEngineScript script;
        script.setName("staging.js");
        script.setSourceCode(history.readAll());
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setInjectionPoint(QWebEngineScript::DocumentCreation);
        profile_->scripts()->insert(script);
    }

    {
        QWebEngineScript script;
        script.setName("qtio.js");
        script.setSourceCode(
            "new QWebChannel(window.qt.webChannelTransport, function(channel) "
            "{\n"
            "  window.qtio = channel.objects.qtio;\n"
            "});");
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setInjectionPoint(QWebEngineScript::DocumentReady);
        script.setRunsOnSubFrames(false);
        profile_->scripts()->insert(script);
    }

    setPage(new QWebEnginePage(profile_, this));

    Q_ASSERT(!io_);
    io_ = new DiffIO(this, page());
    channel_->registerObject(QStringLiteral("qtio"), io_);

    page()->setWebChannel(channel_);
    show();

    QFile diffHtml(htmlRoot.absoluteFilePath("staging.html"));
    if (!diffHtml.open(QIODevice::ReadOnly))
        throw std::runtime_error("Missing staging html file");

    setHtml(diffHtml.readAll());

    return true;
}

void DiffView::onShowDiff(const Diff& diff, bool staging) {
    Q_ASSERT(io_);
    io_->setActiveDiff(diff);

    auto contexts = diff.contexts();
    QJsonArray items;
    for (size_t i = 0; i < contexts.size(); ++i) {
        const auto& context = contexts[i];
        QJsonObject item;
        item["id"] = QString::number(i);
        if (context.context().isEmpty())
            item["title"] = "Global context";
        else
            item["title"] = context.context();

        QJsonArray lines;
        for (const auto& line : context.lines()) {
            QJsonObject data;
            switch (line.type()) {
            case DiffLine::Inserted:
                data["number"] = QString::number(line.line());
                data["short"] = "add";
                break;
            case DiffLine::Deleted:
                data["number"] = QString::number(line.line());
                data["short"] = "del";
                break;
            default:
                break;
            }

            QJsonArray extra;
            QString code = line.text(); //.toHtmlEscaped();
            if (code.indexOf('\r') >= 0)
                extra.append("CR");
            if (line.missingNewLine())
                extra.append("No newline at end of file");
            data["code"] = code.replace("\r", "");
            lines.append(data);
        }
        item["lines"] = lines;
        items.append(item);
    }

    auto info = diff.fileInfo();
    QString fileInfo;
    if (info.first == info.second)
        fileInfo = info.first.filePath();
    else
        fileInfo = info.first.filePath() + " -> " + info.second.filePath();

    QString json = QJsonDocument(items).toJson(QJsonDocument::Compact);
    QString js =
        QString("var scope = "
                "angular.element(document.getElementById('staging')).scope();\n"
                "scope.setFileInfo('%1');\n"
                "scope.setContexts(%2);\n"
                "scope.$apply();")
            .arg(fileInfo)
            .arg(json);
    page()->runJavaScript(js);
}

} // namespace gitigor
