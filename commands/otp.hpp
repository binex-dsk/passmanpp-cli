#ifndef OTP_HPP
#define OTP_HPP

#include "command.hpp"

class OTP : public Command {
public:
    bool parse() override;

    bool run(passman::PDPPDatabase *db) override;

    /* Database */
    QString path;
    QString keyFile;

    passman::VectorUnion password;

    /* Entry */
    QString name;
};

#endif // OTP_HPP
