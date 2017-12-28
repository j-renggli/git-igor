#include <iostream>


#include <QtWidgets/QApplication>
#include <QtCore/QFile>

#include "mainwin.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
	
  gitigor::MainWin window;
	window.resize(1024, 768);
  //window.setMinimumSize(400, 300);
	if (!window.initialise())
		return 1;
	
	QFile stylesheet(window.style().absoluteFilePath());
  stylesheet.open(QFile::ReadOnly);
  QString sheet = QLatin1String(stylesheet.readAll());

  app.setStyleSheet(sheet);
	
  window.show();
  return app.exec();
}
