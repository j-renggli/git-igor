#include "diff_view.h"

#include <QtWebChannel/QWebChannel>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWebEngineWidgets/QWebEngineScriptCollection>

#include <backend/backend.h>

#include "diff_io.h"

namespace gitkit {

UIDiffView::UIDiffView()
{
    profile_ = new QWebEngineProfile("Git Diff", this);
    channel_ = new QWebChannel(this);
}

UIDiffView::~UIDiffView()
{
}

void UIDiffView::doConnect()
{
    // Set-up web channel
    QFile qwebchannel(":/qtwebchannel/qwebchannel.js");
    if (!qwebchannel.open(QIODevice::ReadOnly) )
        throw std::runtime_error("Missing web channel file");

    {
        QByteArray channelSetup = qwebchannel.readAll();
        QWebEngineScript script;
        script.setName("qwebchannel.js");
        script.setSourceCode(channelSetup);
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setInjectionPoint(QWebEngineScript::DocumentCreation);
        profile_->scripts()->insert(script);
    }

    {
        QWebEngineScript script;
        script.setName("qtio.js");
        script.setSourceCode("new QWebChannel(window.qt.webChannelTransport, function(channel) {"
                             "  window.qtio = channel.objects.qtio;"
                             "});\n");
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setInjectionPoint(QWebEngineScript::DocumentCreation);
        script.setRunsOnSubFrames(false);
        profile_->scripts()->insert(script);
        setPage(new QWebEnginePage(profile_, this));
    }

    Q_ASSERT(!diffio_);
    diffio_ = new UIDiffIO(this, page());
    channel_->registerObject(QStringLiteral("qtio"), diffio_);
    page()->setWebChannel(channel_);
    show();
}

void UIDiffView::onShowDiff(const std::vector<Diff>& diff)
{
    Q_ASSERT(diffio_);
    diffio_->setActiveDiff(diff);

    if (diff.empty())
    {
        Q_ASSERT(false);
        setHtml("");
        return;
    }

    QString html = "<html><head><style>";

    const Backend& backend = Backend::instance();
    const QDir folder = backend.cssFolder();
    QFileInfo style(folder, "diff.css");
    QFile file( style.filePath() );
    if ( file.open(QIODevice::ReadOnly) )
    {
        QTextStream in(&file);

        while(!in.atEnd())
        {
                html += in.readLine();
        }

        file.close();
    }


    html += "</style></head><body>";
    auto contexts = diff.at(0).contexts();

    // Header: file name
    html += "<h1>";
    auto fileInfo = diff.at(0).fileInfo();
    if (fileInfo.first == fileInfo.second)
        html += fileInfo.first.filePath();
    else
        html += fileInfo.first.filePath() + " -> " + fileInfo.second.filePath();
    html += "</h1>";

    for (size_t i = 0; i < contexts.size(); ++i)
    //for (const auto& context : contexts)
    {
        const auto& context = contexts[i];
        html += "<div class='context'>\n<h2>";
        if (context.context().isEmpty())
            html += "Global context";
        else
            html += context.context();
        html += "<button onclick=\"window.qtio.onStageHunk(" + QString::number(i) + ");\">Stage hunk</button>";
        html += "</h2>\n";

        QString prefix;
        QString code;
        for (const auto& line : context.lines())
        {
            QString lineOld;
            QString lineNew;
            QString lineType;
            QString classType;
            switch (line.type())
            {
            case DiffLine::Inserted:
                lineNew = QString::number(line.line());
                lineType = "+";
                classType = " addition";
                break;
            case DiffLine::Deleted:
                lineOld = QString::number(line.line());
                lineType = "-";
                classType = " deletion";
                break;
            default:
                lineType = " ";
                break;
            }
            prefix += lineOld + lineNew + " " + lineType + "\n";
            code += "<code class=\"language-cpp" + classType + "\">";
            code += line.text().toHtmlEscaped();
            if (line.missingNewLine())
                code += "<span extra=\"No newline at end of file\"></span>";
            code += "</code>\n";
        }

        html += "<div class=\"diff\"><pre class=\"info\">" + prefix + "</pre><pre>" + code + "</pre></div></div>";
    }

    html += "</body></html>";
    setHtml(html);
}

}
