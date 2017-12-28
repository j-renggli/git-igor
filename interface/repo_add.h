#pragma once

#include <QtCore/QDir>
#include <QtWidgets/QDialog>

class QCheckBox;
class QFileDialog;
class QLineEdit;
class QPushButton;
class QTextEdit;

namespace gitigor {
	
class UIRepoAdd : public QDialog
{
  Q_OBJECT

public:
  UIRepoAdd(bool isRepository);
  virtual ~UIRepoAdd();
	
public:
	void initialise();
	
	QString name() const;
	QDir path() const;
	
private slots:
	void onNameModified(const QString& text);
	void onPathModified(const QString& text);
	void onRemoteModified(const QString& text);
	void updateUI();
	void onCommit();
	
	void onSelectPath();
	void onSelectRemote();
	
private:
	const bool isRepository_;
	
	QLineEdit* name_;
	QLineEdit* path_;
	QLineEdit* remote_;
	QCheckBox* initRemote_;
	QPushButton* pathButton_;
	QPushButton* remoteButton_;
	QFileDialog* chooseFolder_;
	QTextEdit* info_;
	
	QPushButton* cancel_;
	QPushButton* commit_;
};

}
