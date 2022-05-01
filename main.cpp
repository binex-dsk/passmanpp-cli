#include <QCoreApplication>
#include <QCommandLineParser>
#include <QSqlError>
#include <QFile>
#include <QDebug>

#include <passman/constants.hpp>
#include <passman/extra.hpp>
#include <passman/pdpp_database.hpp>

#include "commands/mkdb.hpp"
#include "commands/info.hpp"
#include "commands/edit.hpp"
#include "commands/mkentry.hpp"
#include "commands/rm.hpp"
#include "commands/get.hpp"

//#include <botan/bigint.h>
//#include <passman/constants.hpp>

int main(int argc, char** argv) {
    QCoreApplication app (argc, argv);
    QCoreApplication::setApplicationName(QObject::tr("passman++"));
    QCoreApplication::setApplicationVersion("1.0.0");

    passman::db.open();
    passman::PDPPDatabase *database = new passman::PDPPDatabase();
    // Database path is set up later, in each command's run() function

    if (app.arguments().length() > 1) {
        QString c = app.arguments().at(1);
        QStringList cmdArgs = app.arguments();
        cmdArgs.removeFirst();
        if (c == "mkentry") {
            MkEntry *mkEntry = new MkEntry();
            mkEntry->parse();
            return mkEntry->run(database);
        } else if (c == "modify") {
            QCommandLineParser modifyParser;
            modifyParser.setApplicationDescription(QObject::tr("modify: Modify an entry."));
            modifyParser.addHelpOption();
            modifyParser.addVersionOption();

            modifyParser.addPositionalArgument("name", "Name of the entry.");
            QCommandLineOption entryNameOption(QStringList() << "n" << "new-name", QObject::tr("New name of this entry."), QObject::tr("name"), QObject::tr(""));
            QCommandLineOption entryEmailOption(QStringList() << "e" << "email", QObject::tr("New email of this entry."), QObject::tr("email"), QObject::tr(""));
            QCommandLineOption entryUrlOption(QStringList() << "u" << "url", QObject::tr("New URL of this entry."), QObject::tr("url"), QObject::tr(""));
            QCommandLineOption entryNotesOption(QStringList() << "N" << "notes", QObject::tr("New notes of this entry."), QObject::tr("notes"), QObject::tr(""));
            QCommandLineOption entryPasswordOption(QStringList() << "p" << "password", QObject::tr("New password of this entry (IT IS UNSAFE TO PASS THIS DIRECTLY: prefer command substitution, or type it in the prompt)"), QObject::tr("password"), QObject::tr(""));

            modifyParser.addOptions({entryEmailOption, entryUrlOption, entryNotesOption, entryPasswordOption});

            modifyParser.process(cmdArgs);

            modifyParser.showHelp();
        } else if (c == "get") {
            Get *get = new Get();
            get->parse();
            return get->run(database);
        } else if (c == "rm") {
            RM *rm = new RM();
            rm->parse();
            return rm->run(database);
        } else if (c == "mkdb") {
            MkDb *mkdb = new MkDb();
            mkdb->parse();
            return mkdb->run(database);
        } else if (c == "edit") {
            Edit *edit = new Edit();
            edit->parse();
            return edit->run(database);
        } else if (c == "info") {
            Info *info = new Info();
            info->parse();
            return info->run(database);
        } else {
            QCommandLineParser parser;
            parser.setApplicationDescription(QObject::tr("A simple, minimal, and just as powerful and secure password manager.\n\n"
                    "This program is free software: you can redistribute it and/or modify\n"
                    "it under the terms of the GNU General Public License as published by\n"
                    "the Free Software Foundation, either version 3 of the License, or\n"
                    "(at your option) any later version.\n\n"

                    "This program is distributed in the hope that it will be useful,\n"
                    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                    "GNU General Public License for more details.\n\n"

                    "You should have received a copy of the GNU General Public License\n"
                    "along with this program.  If not, see <https://www.gnu.org/licenses/>."));
            parser.addHelpOption();
            parser.addVersionOption();

            QCommandLineOption debugOption(QStringList() << "D" << "debug", QObject::tr("Activate debug mode for more output."));
            QCommandLineOption verboseOption(QStringList() << "V" << "verbose", QObject::tr("Activate verbose mode for more output."));
            parser.addPositionalArgument("command", "Command to execute");

            QCommandLineOption listOption(QStringList() << "l" << "list", QObject::tr("List available commands."));

            parser.addOptions({debugOption, verboseOption, listOption});

            parser.process(app);

            if (parser.isSet(listOption)) {
                qInfo() << "Commands:\nmkentry\nmodify\nget\nrm\nmkdb\nedit\ninfo";
            }
        }
    } else {
        QCommandLineParser parser;
        parser.setApplicationDescription(passman::tr("A simple, minimal, and just as powerful and secure password manager.\n\n"
                    "This program is free software: you can redistribute it and/or modify\n"
                    "it under the terms of the GNU General Public License as published by\n"
                    "the Free Software Foundation, either version 3 of the License, or\n"
                    "(at your option) any later version.\n\n"

                    "This program is distributed in the hope that it will be useful,\n"
                    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                    "GNU General Public License for more details.\n\n"

                    "You should have received a copy of the GNU General Public License\n"
                    "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n\n"

                    "passman++ is built on libpassman, version " + passman::Constants::libpassmanVersion + ".\n"
                    "For more information on libpassman, see " + passman::Constants::libpassmanGithub + "."));
        parser.addHelpOption();
        parser.addVersionOption();

        QCommandLineOption debugOption(QStringList() << "D" << "debug", QObject::tr("Activate debug mode for more output."));
        QCommandLineOption verboseOption(QStringList() << "V" << "verbose", QObject::tr("Activate verbose mode for more output."));
        parser.addPositionalArgument("command", "Command to execute");

        parser.addOptions({debugOption, verboseOption});

        parser.process(app);

        parser.showHelp();
    }


    //passman::db.open();

    //Database *database = new Database(mainWindow);

    //app.setProperty("debug", QVariant(parser.isSet(debugOption)));
    //app.setProperty("verbose", QVariant(parser.isSet(verboseOption)));

    return app.exec();
}
