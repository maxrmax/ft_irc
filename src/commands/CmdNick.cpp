#include "../../includes/cmdNick.hpp"
#include "../../includes/Client.hpp"

void CmdNick::execute(Server&, Client& client, const ParsedCommand& cmd)
{
    if (cmd.params.empty())
        return;
// TODO: check for valid nicknames + add error handling + add to client.cpp
    client.setNick(cmd.params[0]);
}
