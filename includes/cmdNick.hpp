#ifndef CMDNICK_HPP
#define CMDNICK_HPP
#include "Command.hpp"

class CmdNick : public Command {
public:
    void execute(Server& server,
                 Client& client,
                 const ParsedCommand& cmd);
};
#endif // CMDNICK_HPP