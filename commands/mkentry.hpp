#include "command.hpp"

class MkEntry : public Command {
public:
    bool parse() override;

    QCommandLineOption entryEmailOption;
    QCommandLineOption entryUrlOption;
    QCommandLineOption entryPasswordOption;
    QCommandLineOption entryNotesOption;
}
