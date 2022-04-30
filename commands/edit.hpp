#ifndef EDIT_HPP
#define EDIT_HPP

#include "command.hpp"

class Edit : public Command {
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
    passman::VectorUnion keyFilePath;
};

#endif // EDIT_HPP
