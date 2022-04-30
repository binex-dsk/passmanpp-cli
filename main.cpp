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

//#include <botan/bigint.h>
//#include <passman/constants.hpp>

int main(int argc, char** argv) {
    QCoreApplication app (argc, argv);
    QCoreApplication::setApplicationName(QObject::tr("passman++"));
    QCoreApplication::setApplicationVersion("1.0.0");

    passman::PDPPDatabase *database = new passman::PDPPDatabase();
    // Database path is set up later, in each command's run() function

    if (app.arguments().length() > 1) {
        QString c = app.arguments().at(1);
        QStringList cmdArgs = app.arguments();
        cmdArgs.removeFirst();
        if (c == "mkentry") {
            QCommandLineParser mkEntryParser;
            mkEntryParser.setApplicationDescription(QObject::tr("mkentry: Create an entry."));
            mkEntryParser.addHelpOption();
            mkEntryParser.addVersionOption();

            mkEntryParser.addPositionalArgument("name", "Name of the entry.");
            QCommandLineOption entryEmailOption(QStringList() << "e" << "email", QObject::tr("Email of this entry."), QObject::tr("email"), QObject::tr(""));
            QCommandLineOption entryUrlOption(QStringList() << "u" << "url", QObject::tr("URL of this entry."), QObject::tr("url"), QObject::tr(""));
            QCommandLineOption entryNotesOption(QStringList() << "n" << "notes", QObject::tr("Notes of this entry."), QObject::tr("notes"), QObject::tr(""));
            QCommandLineOption entryPasswordOption(QStringList() << "p" << "password", QObject::tr("Password of this entry (IT IS UNSAFE TO PASS THIS DIRECTLY: prefer command substitution, or type it in the prompt)"), QObject::tr("password"), QObject::tr(""));

            mkEntryParser.addOptions({entryEmailOption, entryUrlOption, entryNotesOption, entryPasswordOption});

            mkEntryParser.process(cmdArgs);

            if (mkEntryParser.positionalArguments().size() < 1) {
                qWarning() << "Entry name must be provided.";
                mkEntryParser.showHelp();
            } else {
                qInfo() << "name:" << mkEntryParser.positionalArguments().at(0);
                qInfo() << "email:" << mkEntryParser.value("email");
                qInfo() << "url:" << mkEntryParser.value("url");
                qInfo() << "notes:" << mkEntryParser.value("notes");
                qInfo() << "password:" << mkEntryParser.value("password");
            }
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
            QCommandLineParser getParser;
            getParser.setApplicationDescription(QObject::tr("get: Get details of an entry."));
            getParser.addHelpOption();
            getParser.addVersionOption();

            getParser.addPositionalArgument("name", "Name of the entry.");
            QCommandLineOption entryEmailOption(QStringList() << "e" << "email", QObject::tr("Get the email of this entry."));
            QCommandLineOption entryUrlOption(QStringList() << "u" << "url", QObject::tr("Get the URL of this entry."));
            QCommandLineOption entryNotesOption(QStringList() << "n" << "notes", QObject::tr("Get the notes of this entry."));
            QCommandLineOption entryPasswordOption(QStringList() << "p" << "password", QObject::tr("Get the password of this entry."));

            getParser.addOptions({entryEmailOption, entryUrlOption, entryNotesOption, entryPasswordOption});

            getParser.process(cmdArgs);

            getParser.showHelp();
        } else if (c == "rm") {
            QCommandLineParser rmParser;
            rmParser.setApplicationDescription(QObject::tr("rm: Remove an entry."));
            rmParser.addHelpOption();
            rmParser.addVersionOption();

            rmParser.addPositionalArgument("name", "Name of the entry.");
            QCommandLineOption forceOption(QStringList() << "f" << "force", QObject::tr("Force confirmation (useful for scripts)"));
            rmParser.addOption(forceOption);

            rmParser.process(cmdArgs);

            rmParser.showHelp();
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
