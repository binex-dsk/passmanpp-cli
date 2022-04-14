#include <QCommandLineOption>
#include <QstringList>

#include "mkentry.hpp"
#include <passman/extra.hpp>

bool MkEntry::parse() {
    m_parser.setApplicationDescription(passman::tr("mkentry: Create an entry."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();

    m_parser.addPositionalArgument("name", "Name of the entry.");
    QCommandLineOption entryEmailOption(QStringList() << "e" << "email", passman::tr("Email of this entry."), passman::tr("email"), passman::tr(""));
    QCommandLineOption entryUrlOption(QStringList() << "u" << "url", passman::tr("URL of this entry."), passman::tr("url"), passman::tr(""));
    QCommandLineOption entryNotesOption(QStringList() << "n" << "notes", passman::tr("Notes of this entry."), passman::tr("notes"), passman::tr(""));
    QCommandLineOption entryPasswordOption(QStringList() << "p" << "password", passman::tr("Password of this entry (IT IS UNSAFE TO PASS THIS DIRECTLY: prefer command substitution, or type it in the prompt)"), passman::tr("password"), passman::tr(""));

    m_parser.addOptions({entryEmailOption, entryUrlOption, entryNotesOption, entryPasswordOption});

    m_parser.process(qApp->arguments());
}
