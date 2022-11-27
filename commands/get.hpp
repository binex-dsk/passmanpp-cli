#ifndef GET_HPP
#define GET_HPP

#include "command.hpp"

class Get : public Command {
public:
    bool parse() override;

    bool run(passman::PDPPDatabase *db) override;

    /* Database */
    QString path;
    QString keyFile;

    passman::VectorUnion password;

    /* Entry */
    QString name;
    bool email;
    bool url;
    bool notes;
    bool entryPassword;
    bool otp;

};

#endif // GET_HPP
