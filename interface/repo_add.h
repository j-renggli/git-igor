#pragma once

#include <QtWidgets/QDialog>

class QCheckBox;
class QLineEdit;
class QPushButton;
class QTextEdit;

namespace gitkit {
	
class UIRepoAdd : public QDialog
{
  Q_OBJECT

public:
  UIRepoAdd();
  virtual ~UIRepoAdd();
	
public:
	void initialise(bool withPath);
	
private slots:
	void onNameModified(const QString& text);
	void onPathModified();
	void onSourceModified();
	void updateUI();
	
private:
	QLineEdit* name_;
	QLineEdit* path_;
	QLineEdit* remote_;
	QCheckBox* initRemote_;
	QPushButton* pathButton_;
	QPushButton* remoteButton_;
	QTextEdit* info_;
	
	QPushButton* cancel_;
	QPushButton* commit_;
};

}
