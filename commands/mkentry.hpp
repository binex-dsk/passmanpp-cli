#ifndef MKENTRY_HPP
#define MKENTRY_HPP

#include "command.hpp"
#include <passman/field.hpp>

class MkEntry : public Command {
public:
    bool parse() override;

    bool run(passman::PDPPDatabase *db) override;

    /* Database */
    QString path;
    QString keyFile;

    passman::VectorUnion password;

    /* Entry */
    QString name;
    QString email;
    QString url;
    QString notes;
    passman::VectorUnion entryPassword;

};

#endif // MKENTRY_HPP
