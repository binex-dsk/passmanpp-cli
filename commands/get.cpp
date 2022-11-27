#include <QCommandLineOption>
#include <QStringList>
#include <QFile>

#include "get.hpp"
#include "../util.hpp"

#include <botan/auto_rng.h>

#include <passman/extra.hpp>
#include <passman/field.hpp>
#include <passman/pdpp_entry.hpp>

bool Get::parse() {
    m_parser.setApplicationDescription(QObject::tr("get: Get details of an entry."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();

    m_parser.addPositionalArgument("path", "Path of the database.");
    m_parser.addPositionalArgument("name", "Name of the entry.");

    QCommandLineOption entryEmailOption(QStringList() << "e" << "email", QObject::tr("Get the email of this entry."));
    QCommandLineOption entryUrlOption(QStringList() << "u" << "url", QObject::tr("Get the URL of this entry."));
    QCommandLineOption entryNotesOption(QStringList() << "n" << "notes", QObject::tr("Get the notes of this entry."));
    QCommandLineOption entryPasswordOption(QStringList() << "P" << "password", QObject::tr("Get the password of this entry."));
    QCommandLineOption entryOTPOption(QStringList() << "o" << "otp", QObject::tr("Get the OTP URI of this entry. If you need an OTP value, use the otp comamnd."));

    QCommandLineOption passwordOption(QStringList() << "p" << "database-password", QObject::tr("Password of the database"), QObject::tr("password"), QObject::tr(""));
    QCommandLineOption keyFileOption(QStringList() << "k" << "key-file", QObject::tr("Key file for the database"), QObject::tr("file"), QObject::tr(""));

    m_parser.addOptions({entryEmailOption, entryUrlOption, entryNotesOption, entryPasswordOption, entryOTPOption, passwordOption, keyFileOption});

    m_parser.process(qApp->arguments());

    QStringList posArgs = m_parser.positionalArguments();
    if (posArgs.length() < 2) {
        qCritical() << "Database path must be provided.";
        std::exit(1);
    }

    path = posArgs.at(1);

    name = posArgs.at(2);

    email = m_parser.isSet(entryEmailOption);
    url = m_parser.isSet(entryUrlOption);
    notes = m_parser.isSet(entryNotesOption);
    entryPassword = m_parser.isSet(entryPasswordOption);
    otp = m_parser.isSet(entryOTPOption);

    password = m_parser.value(passwordOption);
    if (password.asQStr().isEmpty()) {
        qCritical() << "Password must be provided.";
        std::exit(1);
    }

    keyFile = m_parser.value(keyFileOption);

    return true;
}

bool Get::run(passman::PDPPDatabase *db) {
    db->path = path;

    db->parse();

    if (db->keyFile && keyFile.isEmpty()) {
        qCritical() << "Key file must be provided.";
        std::exit(1);
    }
    db->keyFilePath = keyFile;

    // -1 if none set, 0 if 1 is set, > 0 if multiple are set.
    int set = -1 + (url + email + notes + entryPassword);

    // Verify our password, load database's data, and then do the rest.
    if (db->open(password.asQStr(), keyFile)) {
        passman::PDPPEntry *entry = db->entryNamed(name);
        if (!entry || m_parser.positionalArguments().length() < 3) {
            std::cout << "Available entries:" << std::endl;
            for (passman::PDPPEntry *e : db->entries()) {
                std::cout << e->name().toStdString() << std::endl;
            }
        } else {
            QString toPrint;

            toPrint += (email || set == -1 ? ((set != 0 ? "Email: " : "") + entry->fieldNamed("email")->dataStr() + "\n") : "");
            toPrint += (url || set == -1 ? ((set != 0 ? "URL: " : "") + entry->fieldNamed("url")->dataStr() + "\n") : "");
            toPrint += (notes || set == -1 ? ((set != 0 ? "Notes: " : "") + entry->fieldNamed("notes")->dataStr() + "\n") : "");
            toPrint += (entryPassword || set == -1 ? ((set != 0 ? "Password: " : "") + entry->fieldNamed("password")->dataStr() + "\n") : "");
            toPrint += (otp || set == -1 ? ((set != 0 ? "OTP URI: " : "") + entry->fieldNamed("otp")->dataStr() + "\n") : "");

            std::cout << toPrint.toStdString();
        }
    }

    return true;
}
