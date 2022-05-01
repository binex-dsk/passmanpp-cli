#include <QCommandLineOption>
#include <QStringList>
#include <QFile>

#include "rm.hpp"
#include "../util.hpp"

#include <botan/auto_rng.h>

#include <passman/extra.hpp>
#include <passman/field.hpp>
#include <passman/pdpp_entry.hpp>

bool RM::parse() {
    m_parser.setApplicationDescription(QObject::tr("rm: Remove an entry."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();

    m_parser.addPositionalArgument("path", "Path of the database.");
    m_parser.addPositionalArgument("name", "Name of the entry.");

    QCommandLineOption forceOption(QStringList() << "f" << "force", QObject::tr("Force confirmation (useful for scripts)"));

    QCommandLineOption passwordOption(QStringList() << "p" << "database-password", QObject::tr("Password of the database"), QObject::tr("password"), QObject::tr(""));
    QCommandLineOption keyFileOption(QStringList() << "k" << "key-file", QObject::tr("Key file for the database"), QObject::tr("file"), QObject::tr(""));

    m_parser.addOptions({forceOption, passwordOption, keyFileOption});

    m_parser.process(qApp->arguments());

    force = m_parser.isSet(forceOption);

    QStringList posArgs = m_parser.positionalArguments();
    if (posArgs.length() < 2) {
        qCritical() << "Database path must be provided.";
        std::exit(1);
    }

    path = posArgs.at(1);

    if (posArgs.length() < 3) {
        qCritical() << "Entry name must be provided.";
        std::exit(1);
    }

    name = posArgs.at(2);

    password = m_parser.value(passwordOption);
    if (password.asQStr().isEmpty()) {
        qCritical() << "Password must be provided.";
        std::exit(1);
    }

    keyFile = m_parser.value(keyFileOption);

    return true;
}

bool RM::run(passman::PDPPDatabase *db) {
    db->path = path;

    db->parse();

    if (db->keyFile && keyFile.isEmpty()) {
        qCritical() << "Key file must be provided.";
        std::exit(1);
    }
    db->keyFilePath = keyFile;

    // Verify our password, load database's data, and then remove the entry.
    if (db->open(password.asQStr(), keyFile)) {
        passman::PDPPEntry *entry = db->entryNamed(name);
        if (entry) {
            if (!force) {
                std::cout << "Are you sure you want to delete this entry? [yn] ";
                std::string choice;
                std::cin >> choice;

                if (choice == "n" || choice == "N") {
                    return false;
                }
            }
            db->removeEntry(entry);
            db->save();
        } else {
            qCritical() << "An entry with this name does not exist.";
        }
    }

    return true;
}
