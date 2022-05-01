#include <QCommandLineOption>
#include <QStringList>
#include <QFile>

#include "modify.hpp"
#include "../util.hpp"

#include <botan/auto_rng.h>

#include <passman/extra.hpp>
#include <passman/field.hpp>
#include <passman/pdpp_entry.hpp>

bool Modify::parse() {
    m_parser.setApplicationDescription(QObject::tr("modify: Modify an entry."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();

    m_parser.addPositionalArgument("path", "Path of the database.");
    m_parser.addPositionalArgument("name", "Name of the entry.");

    QCommandLineOption entryNameOption(QStringList() << "N" << "new-name", QObject::tr("New name of this entry."), QObject::tr("name"), QObject::tr(""));
    QCommandLineOption entryEmailOption(QStringList() << "e" << "email", QObject::tr("New email of this entry."), QObject::tr("email"), QObject::tr(""));
    QCommandLineOption entryUrlOption(QStringList() << "u" << "url", QObject::tr("New URL of this entry."), QObject::tr("url"), QObject::tr(""));
    QCommandLineOption entryNotesOption(QStringList() << "n" << "notes", QObject::tr("New notes of this entry."), QObject::tr("notes"), QObject::tr(""));
    QCommandLineOption entryPasswordOption(QStringList() << "P" << "password", QObject::tr("New password of this entry (IT IS UNSAFE TO PASS THIS DIRECTLY: prefer command substitution, or type it in the prompt)"), QObject::tr("password"), QObject::tr(""));

    QCommandLineOption passwordOption(QStringList() << "p" << "database-password", QObject::tr("Password of the database"), QObject::tr("password"), QObject::tr(""));
    QCommandLineOption keyFileOption(QStringList() << "k" << "key-file", QObject::tr("Key file for the database"), QObject::tr("file"), QObject::tr(""));

    m_parser.addOptions({entryNameOption, entryEmailOption, entryUrlOption, entryNotesOption, entryPasswordOption, passwordOption, keyFileOption});

    m_parser.process(qApp->arguments());

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

    newName = m_parser.value(entryNameOption);
    email = m_parser.value(entryEmailOption);
    url = m_parser.value(entryUrlOption);
    notes = m_parser.value(entryNotesOption);
    entryPassword = m_parser.value(entryPasswordOption);

    password = m_parser.value(passwordOption);
    if (password.asQStr().isEmpty()) {
        qCritical() << "Password must be provided.";
        std::exit(1);
    }

    keyFile = m_parser.value(keyFileOption);

    return true;
}

bool Modify::run(passman::PDPPDatabase *db) {
    db->path = path;

    db->parse();

    if (db->keyFile && keyFile.isEmpty()) {
        qCritical() << "Key file must be provided.";
        std::exit(1);
    }
    db->keyFilePath = keyFile;

    // Verify our password, load database's data, and then edit the entry.
    if (db->open(password.asQStr(), keyFile)) {
        passman::PDPPEntry *entry = db->entryNamed(name);
        if (entry) {
            newName = m_parser.isSet("new-name") ? newName : name;
            email = m_parser.isSet("email") ? email : entry->fieldNamed("email")->dataStr();
            url = m_parser.isSet("url") ? url : entry->fieldNamed("url")->dataStr();
            notes = m_parser.isSet("notes") ? notes : entry->fieldNamed("notes")->dataStr();
            entryPassword = m_parser.isSet("password") ? password : entry->fieldNamed("password")->dataStr();

            // Create our fields
            passman::Field *nameField = new passman::Field("Name", newName, QMetaType::QString);
            passman::Field *emailField = new passman::Field("Email", email, QMetaType::QString);
            passman::Field *urlField = new passman::Field("URL", url, QMetaType::QString);
            passman::Field *notesField = new passman::Field("Notes", notes, QMetaType::QByteArray);
            passman::Field *passwordField = new passman::Field("Password", entryPassword, QMetaType::QString);

            // Create the entry
            passman::PDPPEntry *newEntry = new passman::PDPPEntry({nameField, emailField, urlField, notesField, passwordField}, db);

            db->removeEntry(entry);
            db->addEntry(newEntry);
            db->save();
        } else {
            qCritical() << "An entry with this name does not exist.";
        }
    }

    return true;
}
