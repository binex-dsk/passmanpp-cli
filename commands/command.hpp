#include <QCommandLineParser>
#include <QCommandLineOption>
#include <passman/pdpp_database.hpp>

class Command {
public:
    virtual ~Command();

    virtual bool parse();

    virtual bool run(passman::PDPPDatabase *db);

    QCommandLineParser m_parser;
};
