#include <iostream>


#include <QtWidgets/QApplication>
#include <QtCore/QFile>

#include "mainwin.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QFile stylesheet("style.qss");
  stylesheet.open(QFile::ReadOnly);
  QString sheet = QLatin1String(stylesheet.readAll());
  app.setStyleSheet(sheet);

  gitkit::MainWin window;
	window.resize(800, 600);
  //window.setMinimumSize(400, 300);
	if (!window.initialise())
		return 1;
		
  window.show();
  return app.exec();
}
