#include <QCommandLineOption>
#include <QStringList>
#include <QFile>

#include "mkdb.hpp"
#include "../util.hpp"

#include <botan/auto_rng.h>

#include <passman/extra.hpp>
#include <passman/field.hpp>
#include <passman/pdpp_entry.hpp>

bool MkDb::parse() {
    m_parser.setApplicationDescription(QObject::tr("mkdb: Make a new database."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();

    m_parser.addPositionalArgument("path", "Path of the new database.");
    QCommandLineOption keyFileOption(QStringList() << "k" << "key-file", QObject::tr("Key file for this database"), QObject::tr("file"), QObject::tr(""));
    QCommandLineOption hmacOption(QStringList() << "H" << "hmac", QObject::tr("HMAC option for this database (see man passman for options)."), QObject::tr("hmac"), QObject::tr("0"));
    QCommandLineOption hashOption(QStringList() << "a" << "hash", QObject::tr("Password hashing option for this database (see man passman for options)"), QObject::tr("hash"), QObject::tr("0"));
    QCommandLineOption hashItersOption(QStringList() << "i" << "hash-iters", QObject::tr("Password hashing iterations for this database"), QObject::tr("iterations"), QObject::tr("8"));
    QCommandLineOption encryptionOption(QStringList() << "e" << "encryption", QObject::tr("Encryption option for this database (see man passman for options)"), QObject::tr("encryption"), QObject::tr("0"));
    QCommandLineOption memoryUsageOption(QStringList() << "m" << "memory", QObject::tr("Memory usage (in MB) to use with password hashing for this database"), QObject::tr("MB"), QObject::tr("64"));
    QCommandLineOption noCompressOption(QStringList() << "c" << "no-compression", QObject::tr("Disable compression for this database (NOT RECOMMENDED!)"));
    QCommandLineOption nameOption(QStringList() << "n" << "name", QObject::tr("Name of this database"), QObject::tr("name"), QObject::tr(""));
    QCommandLineOption descriptionOption(QStringList() << "d" << "description", QObject::tr("Description of this database"), QObject::tr("description"), QObject::tr(""));
    QCommandLineOption passwordOption(QStringList() << "p" << "password", QObject::tr("Password of this database"), QObject::tr("password"), QObject::tr(""));

    m_parser.addOptions({keyFileOption, hmacOption, hashOption, hashItersOption, encryptionOption, memoryUsageOption, noCompressOption, nameOption, descriptionOption, passwordOption});

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

    return true;
}

bool MkDb::run(passman::PDPPDatabase *db) {
    passman::PDPPEntry *entry = new passman::PDPPEntry({}, db);
    for (passman::Field *f : entry->fields()) {
        f->setData("test");
    }

    db->keyFilePath = keyFile;
    db->keyFile = !keyFile.isEmpty();

    if (db->keyFile && !QFile::exists(keyFile)) {
        mkKeyFile(keyFile);
    }

    db->hmac = hmac;
    db->hash = hash;
    db->hashIters = hashIters;
    db->encryption = encryption;
    db->memoryUsage = memoryUsage;
    db->compress = compression;

    db->name = name.isEmpty() ? "No Name" : name;
    db->desc = description.isEmpty() ? "No Description" : description;

    auto encryptor = db->makeKdf()->makeEncryptor();

    Botan::AutoSeeded_RNG rng;

    db->ivLen = encryptor->default_nonce_length();
    db->iv = rng.random_vec(db->ivLen);

    db->passw = db->makeKdf()->transform(password);

    db->path = path;

    db->save();

    std::cout << "COPE";
    return true;
}
