#include "view.h"

#include <memory>
#include <iostream>

#include <QtCore/QObject>

#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebView>
//#include <QtWebKitWidgets/QWebEngineView>

#include <backend/backend.h>

#include <gkassert.h>

namespace gitkit {

UIView::UIView()
{
	//index_.reset(new QTreeView(this));
	//workTree_.reset(new QTreeView(this));
	view_.reset(new QWebView(this));
}

UIView::~UIView()
{
}
	
void UIView::doConnect()
{
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	connect(view_->page()->mainFrame(), &QWebFrame::javaScriptWindowObjectCleared, this, &UIView::onInjectBackend);
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
	view_->page()->mainFrame()->addToJavaScriptWindowObject(QString("backend"), &Backend::instance());
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
        html += "<div class='context'>\n";
        html += "<h2>Line ";
        html += context.startLineOld();
        html += " / ";
        html += context.startLineNew();
        html += "</h1>\n";
        html += "<pre>\n";

        for (const auto& line : context.linear())
        {
            html += "<code class=\"language-cpp";
            switch (line.type())
            {
            case DiffLine::Inserted:
                html += " addition";
                break;
            case DiffLine::Deleted:
                html += " deletion";
                break;
            default:
                break;
            }
            html += "\">" + line.line().toHtmlEscaped() + "</code>\n";
        }

        html += "</pre></div>\n";
    }

    html += "</body></html>";
/*
	html += "</style></head><body><pre>";
	QStringList lines = diff.at(0).lines();
	for (int i = 0; i < lines.length(); ++i)
	{
		QString line = lines.at(i);
		if (line.isEmpty())
		{
			html += "<code />\n";
			continue;
		}
		
		html += "<code class=\"language-cpp";
		if (line[0] == '+')
			html += " addition";
		if (line[0] == '-')
			html += " deletion";
        html += "\">" + line.mid(1).toHtmlEscaped() + "</code>\n";
	}
    html += "</pre></body></html>";*/
	view_->setHtml(html);
}

}
