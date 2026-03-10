#pragma once

#include "command.hpp"

class CmdMode : public Command
{
public:
    void execute(Server &server,
                 ClientUser &clientUser,
                 const ParsedCommand &cmd);
};
