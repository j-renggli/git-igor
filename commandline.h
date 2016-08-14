#pragma once

#include <boost/filesystem.hpp>

#include <QtWidgets/QLineEdit>

namespace ccc {

class CommandLine : public QLineEdit
{
  Q_OBJECT

public:
  CommandLine();
  ~CommandLine();

  void tryAutocomplete();

public:
  void clear();

  const boost::filesystem::path& getCwd() const { return cwd_; }
  void setCwd(const boost::filesystem::path& cwd);

  const boost::filesystem::path& getHome() const { return home_; }
  void setHome(const boost::filesystem::path& home);

  /// Sets a specific additinal command (cd, ...)
  void setCommand(const std::string& command);
  /// Returns the command as in field
  std::string getCommand() const;
  /// Returns the command as separate arguments for use in execl & similar
  /// First item contains full path of executable
  /// @return true if command found, false otherwise
  bool getCommandArgs(std::vector<std::string>& arguments) const;

private:
  bool focusNextPrevChild(bool next) override { return false; }

  void keyPressEvent(QKeyEvent* event) override;

//  void returnPressed() override;

  void setPathVars();

private slots:
  void onTextEdited(const QString& text);

private:
  boost::filesystem::path cwd_;
  boost::filesystem::path home_;
  QStringList pathVars_;
  QStringList cwdVars_;

  std::map<std::string, std::string> fullpaths_;

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
