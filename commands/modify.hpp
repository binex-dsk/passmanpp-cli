#ifndef MODIFY_HPP
#define MODIFY_HPP

#include "command.hpp"
#include <passman/field.hpp>

class Modify : public Command {
public:
    bool parse() override;

    bool run(passman::PDPPDatabase *db) override;

    /* Database */
    QString path;
    QString keyFile;

    passman::VectorUnion password;

    /* Entry */
    QString name;
    QString newName;
    QString email;
    QString url;
    QString notes;
    passman::VectorUnion entryPassword;

};

#endif // MODIFY_HPP
