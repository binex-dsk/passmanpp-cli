#include <QCommandLineOption>
#include <QStringList>
#include <QFile>

#include "otp.hpp"
#include "../util.hpp"

#include <passman/pdpp_entry.hpp>
#include <passman/2fa.hpp>

bool OTP::parse() {
    m_parser.setApplicationDescription(QObject::tr("otp: Get an OTP value of an entry."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();

    m_parser.addPositionalArgument("path", "Path of the database.");
    m_parser.addPositionalArgument("name", "Name of the entry.");

    QCommandLineOption passwordOption(QStringList() << "p" << "database-password", QObject::tr("Password of the database"), QObject::tr("password"), QObject::tr(""));
    QCommandLineOption keyFileOption(QStringList() << "k" << "key-file", QObject::tr("Key file for the database"), QObject::tr("file"), QObject::tr(""));

    m_parser.addOptions({passwordOption, keyFileOption});

    m_parser.process(qApp->arguments());

    QStringList posArgs = m_parser.positionalArguments();
    if (posArgs.length() < 2) {
        qCritical() << "Database path must be provided.";
        std::exit(1);
    }

    path = posArgs.at(1);

    name = posArgs.at(2);

    password = m_parser.value(passwordOption);
    if (password.asQStr().isEmpty()) {
        qCritical() << "Password must be provided.";
        std::exit(1);
    }

    keyFile = m_parser.value(keyFileOption);

    return true;
}

bool OTP::run(passman::PDPPDatabase *db) {
    db->path = path;

    db->parse();

    if (db->keyFile && keyFile.isEmpty()) {
        qCritical() << "Key file must be provided.";
        std::exit(1);
    }
    db->keyFilePath = keyFile;

    // Verify our password, load database's data, and then do the rest.
    if (db->open(password.asQStr(), keyFile)) {
        passman::PDPPEntry *entry = db->entryNamed(name);
        if (!entry || m_parser.positionalArguments().length() < 3) {
            std::cout << "Available entries:" << std::endl;
            for (passman::PDPPEntry *e : db->entries()) {
                std::cout << e->name().toStdString() << std::endl;
            }
        } else {
            passman::VectorUnion otpUri = entry->fieldNamed("otp")->data();
            if (!otpUri.empty()) {
                try {
                    passman::OTP *otp = new passman::OTP(otpUri);
                    otp->validate();

                    std::cout << otp->code().toStdString() << std::endl;
                } catch (std::exception &e) {
                    qCritical() << e.what();
                    return false;
                }
            } else {
                qCritical() << "This entry has no OTP configured.";
                return false;
            }
        }
    }

    return true;
}
