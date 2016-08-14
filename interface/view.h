#pragma once

#include <memory>

#include <QtWidgets/QWidget>

//class QTreeView;
class QWebView;
//class QWebEngineView; // TODO: REPLACE QWebView with QWebEngineView !

namespace gitkit {

class Backend;
	
class UIView : public QWidget
{
  Q_OBJECT

public:
  UIView();
  virtual ~UIView();
	
	void doConnect();
	
private slots:
	void onInjectBackend();
	
private:
	//Q_INVOKABLE void onFetch();
	
signals:
	void update();

	private:
	//std::unique_ptr<QTreeView> index_;
	//std::unique_ptr<QTreeView> workTree_;
	std::unique_ptr<QWebView> view_;

};

}
