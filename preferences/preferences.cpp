#include "preferences.h"

#include <QtCore/QTextStream>

#include <backend/backend.h>

namespace gitigor {

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

        stream << "h1 {" << endl;
        stream << "  background: lightgrey;" << endl;
        stream << "}" << endl << endl;

        stream << "code {" << endl;
        stream << "  tab-size: 4;" << endl; // TODO: user-def
        stream << "}" << endl << endl;

        stream << ".diff {" << endl;
        stream << "  display: flex;" << endl;
        stream << "  font-size: small;" << endl;
        stream << "}" << endl << endl;

        stream << "pre {" << endl;
        stream << "  margin: 0;" << endl;
        stream << "}" << endl << endl;

        stream << ".info {" << endl;
        stream << "  margin-right: 0.5em;" << endl;
        stream << "}" << endl << endl;

        stream << ".hidden {" << endl;
        stream << "  display: none;" << endl;
        stream << "}" << endl << endl;

        stream << "[extra] {" << endl;
        stream << "  margin-left: 0.5em;" << endl;
        stream << "  background: lightcyan;" << endl;
        stream << "}" << endl << endl;

        stream << "[extra]::after {" << endl;
        stream << "  content: attr(extra);" << endl;
        stream << "}" << endl << endl;

        stream << ".context {" << endl;
        stream << "  border: 1px solid lightcyan;" << endl;
        stream << "  margin-top: 1em;" << endl;
        stream << "}" << endl << endl;

        stream << ".context h2 {" << endl;
        stream << "  background: lightcyan;" << endl;
        stream << "  font-size: 1.1em;" << endl;
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
