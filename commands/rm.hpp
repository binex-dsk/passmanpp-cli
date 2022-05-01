#ifndef RM_HPP
#define RM_HPP

#include "command.hpp"
#include <passman/field.hpp>

class RM : public Command {
public:
    bool parse() override;

    bool run(passman::PDPPDatabase *db) override;

    bool force;

    QString name;

    /* Database */
    passman::VectorUnion password;
    QString keyFile;

    QString path;
};

#endif // RM_HPP
