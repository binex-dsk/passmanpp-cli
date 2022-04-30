#include <QCommandLineOption>
#include <QStringList>
#include <QFile>

#include "info.hpp"
#include "../util.hpp"

#include <botan/auto_rng.h>

#include <passman/constants.hpp>
#include <passman/extra.hpp>
#include <passman/field.hpp>
#include <passman/pdpp_entry.hpp>

bool Info::parse() {
    m_parser.setApplicationDescription(passman::tr("info: Get info on a database.\n"
                                                     "If multiple flags are provided, all the selected fields will be shown."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();

    m_parser.addPositionalArgument("path", "Path of the database.");
    QCommandLineOption keyFileOption(QStringList() << "k" << "key-file", passman::tr("Whether or not this database has a key file."));
    QCommandLineOption hmacOption(QStringList() << "H" << "hmac", passman::tr("Get the HMAC option for this database"));
    QCommandLineOption hashOption(QStringList() << "a" << "hash", passman::tr("Get the password hashing option for this database"));
    QCommandLineOption hashItersOption(QStringList() << "i" << "hash-iters", passman::tr("Get the password hashing iterations for this database"));
    QCommandLineOption encryptionOption(QStringList() << "e" << "encryption", passman::tr("Get the encryption option for this database"));
    QCommandLineOption memoryUsageOption(QStringList() << "m" << "memory", passman::tr("Get the memory usage (in MB) used with password hashing for this database"));
    QCommandLineOption compressOption(QStringList() << "c" << "compression", passman::tr("Whether or not this database is compressed."));
    QCommandLineOption nameOption(QStringList() << "n" << "name", passman::tr("Get the name of this database"));
    QCommandLineOption descriptionOption(QStringList() << "d" << "description", passman::tr("Get the description of this database"));

    QCommandLineOption passwordOption(QStringList() << "p" << "password", passman::tr("Password to attempt for decrypting the database."), passman::tr("password"), passman::tr(""));
    QCommandLineOption keyFilePathOption(QStringList() << "K" << "key-file-path", passman::tr("Key file path to attempt for decrypting the database."), passman::tr("path"), passman::tr(""));

    m_parser.addOptions({keyFileOption, hmacOption, hashOption, hashItersOption, encryptionOption, memoryUsageOption, compressOption, nameOption, descriptionOption, passwordOption, keyFilePathOption});

    m_parser.process(qApp->arguments());

    QStringList posArgs = m_parser.positionalArguments();
    if (posArgs.empty()) {
        qCritical() << "Database path must be provided.";
        std::exit(1);
    }

    path = posArgs.at(1);

    keyFile = m_parser.isSet(keyFileOption);
    hmac = m_parser.isSet(hmacOption);
    hash = m_parser.isSet(hashOption);
    hashIters = m_parser.isSet(hashItersOption);
    encryption = m_parser.isSet(encryptionOption);
    memoryUsage = m_parser.isSet(memoryUsageOption);
    compression = m_parser.isSet(compressOption);
    name = m_parser.isSet(nameOption);
    description = m_parser.isSet(descriptionOption);

    password = m_parser.value(passwordOption);
    if (password.asQStr().isEmpty()) {
        qCritical() << "Password must be provided.";
        std::exit(1);
    }

    keyFilePath = m_parser.value(keyFilePathOption);

    return true;
}

bool Info::run(passman::PDPPDatabase *db) {
    db->path = path;

    db->parse();

    if (db->keyFile && keyFilePath.asQStr().isEmpty()) {
        qCritical() << "Key file must be provided.";
        std::exit(1);
    }
    db->keyFilePath = keyFilePath;

    // -1 if none set, 0 if 1 is set, > 0 if multiple are set.
    int set = -1 + (keyFile + hmac + hash + hashIters + encryption + memoryUsage + compression + name + description);

    // Verify our password, load database's data, and then print the stuff out.
    if (db->verify(password)) {
        db->get();

        QString toPrint;

        // Bruh
        // If the option is set or none are set: then we want to print something
        // If no options are set or multiple are: display the field's name
        // Then print the value
        // Effectively, if this is the only option set, print just the value, otherwise,
        // if none are set or multiple are set, display the values with their names.
        toPrint += (name || set == -1 ? ((set != 0 ? "Name: " : "") + db->name.asQStr()) + "\n" : "");
        toPrint += (description || set == -1 ? ((set != 0 ? "Description: " : "") + db->desc.asQStr() + "\n") : "");
        toPrint += (hmac || set == -1 ? ((set != 0 ? "HMAC Function: " : "") + QString::fromStdString(passman::Constants::hmacMatch.at(db->hmac)) + "\n") : "");
        toPrint += (hash || set == -1 ? ((set != 0 ? "Hashing Function: " : "") + QString::fromStdString(passman::Constants::hashMatch.at(db->hash)) + "\n") : "");
        toPrint += (hashIters || set == -1 ? ((set != 0 ? "Hashing Iterations: " : "") + QString::number(db->hashIters) + "\n") : "");
        toPrint += (encryption || set == -1 ? ((set != 0 ? "Encryption Function: " : "") + QString::fromStdString(passman::Constants::encryptionMatch.at(db->encryption)) + "\n") : "");
        toPrint += (memoryUsage || set == -1 ? ((set != 0 ? "Memory Usage (MB): " : "") + QString::number(db->memoryUsage) + "\n") : "");
        toPrint += (compression || set == -1 ? ((set != 0 ? "Compression: " : "") + QString(db->compress ? "On" : "Off") + "\n") : "");

        std::cout << toPrint.toStdString();
    }

    return true;
}
