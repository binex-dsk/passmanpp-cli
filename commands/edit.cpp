#include <QCommandLineOption>
#include <QStringList>
#include <QFile>

#include "edit.hpp"
#include "../util.hpp"

#include <botan/auto_rng.h>

#include <passman/constants.hpp>
#include <passman/extra.hpp>
#include <passman/field.hpp>
#include <passman/pdpp_entry.hpp>

bool Edit::parse() {
    m_parser.setApplicationDescription(QObject::tr("edit: Edit a database."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();

    m_parser.addPositionalArgument("name", "Name of the database.");
    QCommandLineOption keyFileOption(QStringList() << "k" << "key-file", QObject::tr("New key file for this database"), QObject::tr("file"), QObject::tr(""));
    QCommandLineOption hmacOption(QStringList() << "H" << "hmac", QObject::tr("New HMAC option for this database (see man passman for options)."), QObject::tr("hmac"), QObject::tr(""));
    QCommandLineOption hashOption(QStringList() << "a" << "hash", QObject::tr("New password hashing option for this database (see man passman for options)"), QObject::tr("hash"), QObject::tr(""));
    QCommandLineOption hashItersOption(QStringList() << "i" << "hash-iters", QObject::tr("New password hashing iterations for this database"), QObject::tr("iterations"), QObject::tr(""));
    QCommandLineOption encryptionOption(QStringList() << "e" << "encryption", QObject::tr("New encryption option for this database (see man passman for options)"), QObject::tr("encryption"), QObject::tr(""));
    QCommandLineOption memoryUsageOption(QStringList() << "m" << "memory", QObject::tr("New memory usage (in MB) to use with password hashing for this database"), QObject::tr("MB"), QObject::tr(""));
    QCommandLineOption noCompressOption(QStringList() << "c" << "no-compression", QObject::tr("Disable compression for this database (NOT RECOMMENDED!)"));
    QCommandLineOption nameOption(QStringList() << "n" << "name", QObject::tr("New name of this database"), QObject::tr("name"), QObject::tr(""));
    QCommandLineOption descriptionOption(QStringList() << "d" << "description", QObject::tr("New description of this database"), QObject::tr("description"), QObject::tr(""));

    QCommandLineOption passwordOption(QStringList() << "p" << "password", passman::tr("Password to attempt for decrypting the database."), passman::tr("password"), passman::tr(""));
    QCommandLineOption keyFilePathOption(QStringList() << "K" << "key-file-path", passman::tr("Key file path to attempt for decrypting the database."), passman::tr("path"), passman::tr(""));

    m_parser.addOptions({keyFileOption, hmacOption, hashOption, hashItersOption, encryptionOption, memoryUsageOption, noCompressOption, nameOption, descriptionOption, passwordOption, keyFilePathOption});

    m_parser.process(qApp->arguments());

    QStringList posArgs = m_parser.positionalArguments();
    if (posArgs.empty()) {
        qCritical() << "Database path must be provided.";
        std::exit(1);
    }

    path = posArgs.at(1);


    keyFile = m_parser.value(keyFileOption);
    hmac = m_parser.value(hmacOption).toInt();
    hash = m_parser.value(hashOption).toInt();
    hashIters = m_parser.value(hashItersOption).toInt();
    encryption = m_parser.value(encryptionOption).toInt();
    memoryUsage = m_parser.value(memoryUsageOption).toInt();
    compression = !m_parser.isSet(noCompressOption);
    name = m_parser.value(nameOption);
    description = m_parser.value(descriptionOption);

    password = m_parser.value(passwordOption);
    if (password.asQStr().isEmpty()) {
        qCritical() << "Password must be provided.";
        std::exit(1);
    }

    keyFilePath = m_parser.value(keyFilePathOption);

    return true;
}

bool Edit::run(passman::PDPPDatabase *db) {
    db->path = path;

    db->parse();

    if (db->keyFile && keyFilePath.asQStr().isEmpty()) {
        qCritical() << "Key file must be provided.";
        std::exit(1);
    }
    db->keyFilePath = keyFilePath;

    if (db->verify(password)) {
        db->keyFilePath = keyFile;
        db->keyFile = !keyFile.isEmpty();

        if (db->keyFile && !QFile::exists(keyFile)) {
            mkKeyFile(keyFile);
        }

        db->hmac = m_parser.isSet("hmac") ? hmac : db->hmac;
        db->hash = m_parser.isSet("hash") ? hash : db->hash;
        db->hashIters = m_parser.isSet("hash-iters") ? hashIters : db->hashIters;
        db->encryption = m_parser.isSet("encryption") ? encryption : db->encryption;
        db->memoryUsage = m_parser.isSet("memory") ? memoryUsage : db->memoryUsage;
        db->compress = compression;

        db->name = name.isEmpty() ? db->name : name;
        db->desc = description.isEmpty() ? db->desc : description;

        auto encryptor = db->makeKdf()->makeEncryptor();

        Botan::AutoSeeded_RNG rng;

        db->ivLen = encryptor->default_nonce_length();
        db->iv = {};
        for (size_t i = 0; i < db->ivLen; ++i) {
            uint8_t next_byte = rng.next_byte();
            // Having any null bytes in the iv causes a crash.
            if (next_byte == 0) {
                --i;
                continue;
            }
            db->iv.push_back(next_byte);
        }

        db->passw = db->makeKdf()->transform(password);
        db->save();
    }

    return true;
}
