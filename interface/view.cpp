#include "view.h"

#include <memory>
#include <iostream>

#include <QtCore/QObject>

#include <QtWebEngineWidgets/QWebEngineView>

#include <backend/backend.h>

#include <gkassert.h>

namespace gitkit {

UIView::UIView()
{
	//index_.reset(new QTreeView(this));
	//workTree_.reset(new QTreeView(this));
    view_ = new QWebEngineView(this);
}

UIView::~UIView()
{
}
	
void UIView::doConnect()
{
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    //connect(view_->page()->mainFrame(), &QWebFrame::javaScriptWindowObjectCleared, this, &UIView::onInjectBackend);
	view_->setHtml("<html>"

"<head>"
"<title>Git-Kit</title>"
"<script>"
"function doFetch() {"
"  //alert('asdf');\n"
"  console.log(backend);\n"
"  backend.onFetch();\n"
"}"
"</script>"
"</head>"

"<body>"
"Some text"
"<button onclick=\"doFetch();\">with a vengeance</button>"
"</body>"
"</html>"
);
	onInjectBackend();
	view_->show();
}

void UIView::onInjectBackend()
{
    //view_->page()->mainFrame()->addToJavaScriptWindowObject(QString("backend"), &Backend::instance());
}

void UIView::onShowDiff(const std::vector<Diff>& diff)
{
	if (diff.empty())
	{
		ASSERT(false);
		view_->setHtml("");
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

    for (const auto& context : contexts)
    {
        html += "<div class='context'>\n<h2>";
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
            code += line.text().toHtmlEscaped() + "</code>\n";
        }

        html += "<div class=\"diff\"><pre class=\"info\">" + prefix + "</pre><pre>" + code + "</pre></div></div>";

        /*

        html += "<pre>\n";

        html += "<table>\n";

        for (const auto& line : context.lines())
        {
            QString thLine;
            QString thType;
            QString classType;
            switch (line.type())
            {
            case DiffLine::Inserted:
                thLine = QString::number(line.line());
                thType = "+";
                classType = " addition";
                break;
            case DiffLine::Deleted:
                thLine = QString::number(line.line());
                thType = "-";
                classType = " deletion";
                break;
            default:
                thType = " ";
                break;
            }

            html += "<tr class='" + classType + "'>\n";
            html += "<th class='line'>" + thLine + "</th>\n";
            html += "<th class='type'>" + thType + "</th>\n";
            html += "<td><code class=\"language-cpp\">";
            html += line.text().toHtmlEscaped() + "</code></td>\n</tr>\n";
        }

        html += "</table></pre></div>\n";
        */
    }

    html += "</body></html>";
	view_->setHtml(html);
}

}
