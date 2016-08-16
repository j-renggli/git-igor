#include "view.h"

#include <memory>
#include <iostream>

#include <QtCore/QObject>

#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebView>
//#include <QtWebKitWidgets/QWebEngineView>

#include "backend.h"

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
	QStringList lines = diff.at(0).lines();
	for (int i = 0; i < lines.length(); ++i)
	{
		QString line = lines.at(i);
		if (line.isEmpty())
		{
			html += "<div />\n";
			continue;
		}
		
		html += "<div";
		if (line[0] == '+')
			html += " class=\"addition\"";
		if (line[0] == '-')
			html += " class=\"deletion\"";
		html += ">" + line + "</div>\n";
	}
	html += "</body></html>";
	view_->setHtml(html);
}

}
