#pragma once

#include <memory>

#include <QtWidgets/QWidget>

#include <backend/repository.h>

class QWebEngineProfile;
class QWebEngineView;

namespace gitkit {

class Backend;
	
class UIView : public QWidget
{
    Q_OBJECT

public:
    UIView();
    virtual ~UIView();

    void doConnect();

signals:
    void update();

private slots:
    void onInjectBackend();
    void onShowDiff(const std::vector<Diff>& diff);

private:
    //Q_INVOKABLE void onFetch();

private:
    QWebEngineView* view_;
    QWebEngineProfile* profile_;

};

}
