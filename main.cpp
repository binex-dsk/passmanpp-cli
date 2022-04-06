#include <QCoreApplication>
#include <QCommandLineParser>
#include <QSqlError>
#include <QFile>
#include <QDebug>

//#include <botan/bigint.h>
//#include <passman/constants.hpp>

int main(int argc, char** argv) {
    QCoreApplication app (argc, argv);
    QCoreApplication::setApplicationName(QObject::tr("passman++"));
    QCoreApplication::setApplicationVersion("1.0.0");

    //const QStringList args = parser.positionalArguments();

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
            QCommandLineParser mkdbParser;
            mkdbParser.setApplicationDescription(QObject::tr("mkdb: Make a new database."));
            mkdbParser.addHelpOption();
            mkdbParser.addVersionOption();

            mkdbParser.addPositionalArgument("name", "Name of the new database.");
            QCommandLineOption keyFileOption(QStringList() << "k" << "key-file", QObject::tr("Key file for this database"), QObject::tr("file"), QObject::tr(""));
            QCommandLineOption hmacOption(QStringList() << "H" << "hmac", QObject::tr("HMAC option for this database (see man passman for options)."), QObject::tr("hmac"), QObject::tr("0"));
            QCommandLineOption hashOption(QStringList() << "a" << "hash", QObject::tr("Password hashing option for this database (see man passman for options)"), QObject::tr("hash"), QObject::tr("0"));
            QCommandLineOption hashItersOption(QStringList() << "i" << "hash-iters", QObject::tr("Password hashing iterations for this database"), QObject::tr("iterations"), QObject::tr("8"));
            QCommandLineOption encryptionOption(QStringList() << "e" << "encryption", QObject::tr("Encryption option for this database (see man passman for options)"), QObject::tr("encryption"), QObject::tr("0"));
            QCommandLineOption memoryUsageOption(QStringList() << "m" << "memory", QObject::tr("Memory usage (in MB) to use with password hashing for this database"), QObject::tr("MB"), QObject::tr("64"));
            QCommandLineOption noCompressOption(QStringList() << "c" << "no-compression", QObject::tr("Disable compression for this database (NOT RECOMMENDED!)"));
            QCommandLineOption nameOption(QStringList() << "n" << "name", QObject::tr("Name of this database"), QObject::tr("name"), QObject::tr(""));
            QCommandLineOption descriptionOption(QStringList() << "d" << "description", QObject::tr("Description of this database"), QObject::tr("description"), QObject::tr(""));

            mkdbParser.addOptions({keyFileOption, hmacOption, hashOption, hashItersOption, encryptionOption, memoryUsageOption, noCompressOption, nameOption, descriptionOption});

            mkdbParser.process(cmdArgs);

            mkdbParser.showHelp();
        } else if (c == "edit") {
            QCommandLineParser editParser;
            editParser.setApplicationDescription(QObject::tr("edit: Edit a database."));
            editParser.addHelpOption();
            editParser.addVersionOption();

            editParser.addPositionalArgument("name", "Name of the database.");
            QCommandLineOption keyFileOption(QStringList() << "k" << "key-file", QObject::tr("New key file for this database"), QObject::tr("file"), QObject::tr(""));
            QCommandLineOption hmacOption(QStringList() << "H" << "hmac", QObject::tr("New HMAC option for this database (see man passman for options)."), QObject::tr("hmac"), QObject::tr(""));
            QCommandLineOption hashOption(QStringList() << "a" << "hash", QObject::tr("New password hashing option for this database (see man passman for options)"), QObject::tr("hash"), QObject::tr(""));
            QCommandLineOption hashItersOption(QStringList() << "i" << "hash-iters", QObject::tr("New password hashing iterations for this database"), QObject::tr("iterations"), QObject::tr(""));
            QCommandLineOption encryptionOption(QStringList() << "e" << "encryption", QObject::tr("New encryption option for this database (see man passman for options)"), QObject::tr("encryption"), QObject::tr(""));
            QCommandLineOption memoryUsageOption(QStringList() << "m" << "memory", QObject::tr("New memory usage (in MB) to use with password hashing for this database"), QObject::tr("MB"), QObject::tr(""));
            QCommandLineOption noCompressOption(QStringList() << "c" << "no-compression", QObject::tr("Disable compression for this database (NOT RECOMMENDED!)"));
            QCommandLineOption nameOption(QStringList() << "n" << "name", QObject::tr("New name of this database"), QObject::tr("name"), QObject::tr(""));
            QCommandLineOption descriptionOption(QStringList() << "d" << "description", QObject::tr("New description of this database"), QObject::tr("description"), QObject::tr(""));

            editParser.addOptions({keyFileOption, hmacOption, hashOption, hashItersOption, encryptionOption, memoryUsageOption, noCompressOption, nameOption, descriptionOption});

            editParser.process(cmdArgs);

            editParser.showHelp();
        } else if (c == "info") {
            QCommandLineParser infoParser;
            infoParser.setApplicationDescription(QObject::tr("info: Get info on a database.\n"
                                                             "If multiple flags are provided, all the selected fields will be shown."));
            infoParser.addHelpOption();
            infoParser.addVersionOption();

            infoParser.addPositionalArgument("name", "Name of the database.");
            QCommandLineOption keyFileOption(QStringList() << "k" << "key-file", QObject::tr("Whether or not this database has a key file."));
            QCommandLineOption hmacOption(QStringList() << "H" << "hmac", QObject::tr("Get the HMAC option for this database"));
            QCommandLineOption hashOption(QStringList() << "a" << "hash", QObject::tr("Get the password hashing option for this database"));
            QCommandLineOption hashItersOption(QStringList() << "i" << "hash-iters", QObject::tr("Get the password hashing iterations for this database"));
            QCommandLineOption encryptionOption(QStringList() << "e" << "encryption", QObject::tr("Get the encryption option for this database"));
            QCommandLineOption memoryUsageOption(QStringList() << "m" << "memory", QObject::tr("Get the memory usage (in MB) used with password hashing for this database"));
            QCommandLineOption noCompressOption(QStringList() << "c" << "no-compression", QObject::tr("Whether or not this database is compressed."));
            QCommandLineOption descriptionOption(QStringList() << "d" << "description", QObject::tr("Get the description of this database"));

            infoParser.addOptions({keyFileOption, hmacOption, hashOption, hashItersOption, encryptionOption, memoryUsageOption, noCompressOption, descriptionOption});

            infoParser.process(cmdArgs);

            infoParser.showHelp();
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

            parser.addOptions({debugOption, verboseOption});

            parser.process(app);

            parser.showHelp();
        }
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
