#ifndef INFO_HPP
#define INFO_HPP

#include "command.hpp"

class Info : public Command {
public:
    bool parse() override;

    bool run(passman::PDPPDatabase *db) override;

    QString path;

    bool keyFile, hmac, hash, hashIters, encryption, memoryUsage, compression, name, description;

    passman::VectorUnion password;
    passman::VectorUnion keyFilePath;
};

#endif // INFO_HPP
