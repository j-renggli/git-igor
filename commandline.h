#pragma once

#include <boost/filesystem.hpp>

#include <QtGui/QLineEdit>

namespace ccc {

class CommandLine : public QLineEdit
{
  Q_OBJECT

public:
  CommandLine();
  ~CommandLine();

  void tryAutocomplete();

public:
  const boost::filesystem::path& getCwd() const { return cwd_; }
  void setCwd(const boost::filesystem::path& cwd);

  /// Returns the command as in field
  std::string getCommand() const;
  /// Returns the command as separate arguments for use in execl & similar
  std::vector<std::string> getCommandArgs() const;

private:
  bool focusNextPrevChild(bool next) override { return false; }

  void keyPressEvent(QKeyEvent* event) override;

//  void returnPressed() override;

  void setPathVars();

private slots:
  void onTextEdited(const QString& text);

private:
  boost::filesystem::path cwd_;
  QStringList pathVars_;
  QStringList cwdVars_;

  QCompleter* completer_;

  enum eCurrentModel
  {
    kCurrentPath,
    kCurrentArgs,
    kCurrentDir
  };

  eCurrentModel current_;
};

}
