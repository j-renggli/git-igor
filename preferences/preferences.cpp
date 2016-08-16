#include "preferences.h"

#include <QtCore/QTextStream>

#include <interface/backend.h>

namespace gitkit {

Preferences Preferences::s_preferences;

Preferences& Preferences::instance()
{
	return s_preferences;
}

Preferences::Preferences()
{
}

bool Preferences::generateCSS()
{
	const Backend& backend = Backend::instance();
	const QDir folder = backend.cssFolder();
	
	QFileInfo diff(folder, "diff.css");
	QFile file( diff.filePath() );
	if ( file.open(QIODevice::ReadWrite) )
	{
    QTextStream stream( &file );
		stream << "body {" << endl;
		stream << "  font-family: \"Courier New\", Monospace;" << endl;
    stream << "  font-size: 10pt;" << endl;
		stream << "  margin: 0;" << endl;
		stream << "}" << endl << endl;
		
    stream << ".addition {" << endl;
		stream << "  background-color: #eeffee;" << endl;
    stream << "}" << endl << endl;
		
    stream << ".deletion {" << endl;
		stream << "  background-color: #ffeeee;" << endl;
    stream << "}" << endl << endl;
	}
	file.close(); 

	return true;
}

}