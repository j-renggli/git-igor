#include "commandline.h"

#include "cccassert.h"

#include <QtWidgets/QCompleter>
#include <QtGui/QKeyEvent>
#include <QtCore/QStringListModel>

#include <iostream>

namespace ccc {

////////////////////////////////////////////////////////////////

CommandLine::CommandLine()
: current_(kCurrentPath)
{
  setPathVars();

  QStringListModel* model = new QStringListModel(pathVars_);
  completer_ = new QCompleter(model, this);
  setCompleter(completer_);

  setHome(getenv("HOME"));
  setCwd(getenv("PWD"));

  connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
}

////////////////////////////////////////////////////////////////

CommandLine::~CommandLine()
{
  delete completer_;
}

////////////////////////////////////////////////////////////////

void CommandLine::clear()
{
  setText("");
}

////////////////////////////////////////////////////////////////

std::string CommandLine::getCommand() const
{
  return std::string(text().toUtf8().constData());
}

////////////////////////////////////////////////////////////////

bool CommandLine::getCommandArgs(std::vector<std::string>& tokens) const
{
  const std::string original = getCommand();
  const std::string special = "'\"` ";
  tokens.clear();
  size_t begin = 0;
  while (true)
  {
    size_t end = original.find_first_of(special, begin);
    if (end >= original.npos)
    {
      if (end != begin)
      {
        tokens.push_back(original.substr(begin, end - begin));
        break;
      }
    }

    if (original[end] == ' ')
    {
      tokens.push_back(original.substr(begin, end - begin));
      begin = original.find_first_not_of(" ", end);
    } else {
      begin = end+1;
      end = original.find_first_of(original[begin-1], begin);
      tokens.push_back(original.substr(begin, end - begin));
      begin = end+1;
      if (original[begin] == ' ')
        begin = original.find_first_not_of(" ", begin);
    }
  }

  if (tokens.empty())
    return false;

  auto it = fullpaths_.find(tokens[0]);
  //assert(it != fullpaths_.end());
  if (it != fullpaths_.end())
  {
    //tokens[0] = it->second;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////

void CommandLine::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Tab)
  {
    tryAutocomplete();
    return;
  }

  QLineEdit::keyPressEvent(event);
}

////////////////////////////////////////////////////////////////

void CommandLine::onTextEdited(const QString& text)
{
  const eCurrentModel model = text.contains(' ') ? kCurrentArgs : kCurrentPath;
  if (model == current_)
    return;

  current_ = model;
  QStringListModel* list = static_cast<QStringListModel*>(completer_->model());
  if (model == kCurrentPath)
  {
    list->setStringList(pathVars_);
  }
  else
  {
//    QStringList args;
//    args
  }
}

////////////////////////////////////////////////////////////////

//void CommandLine::returnPressed()
//{
//
//}

////////////////////////////////////////////////////////////////

void CommandLine::setCwd(const boost::filesystem::path& cwd)
{
  cwd_ = cwd;
  cwdVars_ = pathVars_;

  for (boost::filesystem::directory_iterator it(cwd); it != boost::filesystem::directory_iterator(); ++it)
  {
    if (is_directory(it->status()))
      continue;

    // TODO: can do better than that...
    boost::filesystem::file_status stat = status(it->path());
    const int exec = stat.permissions() & boost::filesystem::owner_exe;
    if (!exec)
      continue;

    const std::string name = it->path().filename().string();
    fullpaths_[name] = it->path().string();
    cwdVars_ << name.c_str();
  }
}

////////////////////////////////////////////////////////////////

void CommandLine::setHome(const boost::filesystem::path& home)
{
  home_ = home;
}

////////////////////////////////////////////////////////////////

void CommandLine::setPathVars()
{
  pathVars_.clear();

  size_t begin = 0;
  const std::string strPath = getenv("PATH");
  while (true)
  {
    size_t next = strPath.find(':', begin);
    const boost::filesystem::path path = strPath.substr(begin, next - begin);
    begin = next + 1;

    if (!exists(path))
      continue;

    for (boost::filesystem::directory_iterator it(path); it != boost::filesystem::directory_iterator(); ++it)
    {
      if (is_directory(it->status()))
        continue;

      // TODO: can do better than that...
      boost::filesystem::file_status stat = status(it->path());
      const int exec = stat.permissions() & boost::filesystem::others_exe;
      if (!exec)
        continue;

      const std::string name = it->path().filename().string();
      fullpaths_[name] = it->path().string();
      pathVars_ << name.c_str();
    }

    if (next == strPath.npos)
      break;
  }
}

void CommandLine::setCommand(const std::string& command)
{
  pathVars_ << command.c_str();
}

////////////////////////////////////////////////////////////////

void CommandLine::tryAutocomplete()
{
  std::cout << "Autocomplete" << std::endl;
}

////////////////////////////////////////////////////////////////

}
