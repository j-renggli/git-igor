#include <iostream>

#include <QApplication>

#include "mainwin.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  //Q_INIT_RESOURCE();
  ccc::MainWin window;
  window.setMinimumSize(300, 200);
  window.show();
  return app.exec();
}
