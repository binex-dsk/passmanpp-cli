#include <QCommandLineParser>
#include <QCommandLineOption>

class Command {
public:
    Command() = default;
    virtual ~Command() = default;

    virtual bool parse();

    virtual bool run();

    virtual void description();

    QCommandLineParser m_parser;
}
