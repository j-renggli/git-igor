#include "history_view.h"

#include <QtCore/QDebug>
#include <QtWebChannel/QWebChannel>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWebEngineWidgets/QWebEngineScriptCollection>

#include <backend/git/logger.h>
#include <backend/git/process.h>
#include <backend/repository.h>

namespace gitigor {

UIHistoryView::UIHistoryView() {}

UIHistoryView::~UIHistoryView() {}

bool UIHistoryView::initialise() {
    profile_ = new QWebEngineProfile("Git History", this);
    channel_ = new QWebChannel(this);

    // Set-up web channel
    QFile qwebchannel(":/qtwebchannel/qwebchannel.js");
    if (!qwebchannel.open(QIODevice::ReadOnly))
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
    /*
    {
        QWebEngineScript script;
        script.setName("qtio.js");
        script.setSourceCode("new QWebChannel(window.qt.webChannelTransport,
    function(channel) {\n"
                             "  window.qtio = channel.objects.qtio;\n"
                             "  window.wrappers = {\n"
                             "    stageHunk: function(index) {\n"
                             "      var name='diff_'+index;\n"
                             "
    document.getElementById(name).classList.add('hidden');\n"
                             "      window.qtio.onStageHunk(index);\n"
                             "    }\n"
                             "  }\n"
                             "});");
        script.setWorldId(QWebEngineScript::MainWorld);
        script.setInjectionPoint(QWebEngineScript::DocumentReady);
        script.setRunsOnSubFrames(false);
        profile_->scripts()->insert(script);
        setPage(new QWebEnginePage(profile_, this));
    }

    Q_ASSERT(!diffio_);
    diffio_ = new UIDiffIO(this, page());
    channel_->registerObject(QStringLiteral("qtio"), diffio_);
    page()->setWebChannel(channel_);
    show();
    */
    QString html = "<html><head><style>";
    html += "</style></head><body>";
    html += "TODO";
    html += "</body></html>";
    setHtml(html);
    return true;
}

void UIHistoryView::nextItem(const gitigor::LogItem& item) {
    qDebug() << item.id << ": " << item.summary;
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
    // update();
}

void UIHistoryView::update() {
    // TODO: separate thread, update page on the fly (javascript)
    /*    Q_ASSERT(diffio_);
        diffio_->setActiveDiff(diff);

        if (diff.empty())
        {
            Q_ASSERT(false);
            // This can happen if file was reset before a refresh happened
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
            html += fileInfo.first.filePath() + " -> " +
       fileInfo.second.filePath();
        html += "</h1>";

        for (size_t i = 0; i < contexts.size(); ++i)
        {
            const QString diffIndex = QString::number(i);
            const auto& context = contexts[i];
            html += "<div class='context' id='diff_" + diffIndex + "'>\n<h2>";
            if (context.context().isEmpty())
                html += "Global context";
            else
                html += context.context();
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
                code += line.text().toHtmlEscaped().replace("\r", "<span
       extra=\"CR\"></span>");
                if (line.missingNewLine())
                    code += "<span extra=\"No newline at end of file\"></span>";
                code += "</code>\n";
            }

            QString hunk = "<button onclick=\"window.wrappers.stageHunk(" +
       diffIndex + ");\">Stage hunk</button>";

            html += "<div class=\"diff\"><pre class=\"info\">" + prefix +
       "</pre><pre>" + code + "</pre></div>" + hunk + "</div>";
        }

        html += "</body></html>";
        setHtml(html);*/
}

} // namespace gitigor
