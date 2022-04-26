#ifndef MKDB_HPP
#define MKDB_HPP

#include "command.hpp"

class MkDb : public Command {
public:
    bool parse() override;

    bool run(passman::PDPPDatabase *db) override;

    QString path;

    QString keyFile;
    int hmac;
    int hash;
    int hashIters;
    int encryption;
    int memoryUsage;
    bool compression;
    QString name;
    QString description;

    passman::VectorUnion password;
};

#endif // MKDB_HPP
