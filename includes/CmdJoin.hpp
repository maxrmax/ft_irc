#pragma once

#include "command.hpp"

class CmdJoin : public Command
{
public:
    void execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd) override;
};
