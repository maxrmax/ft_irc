#include "CommandDispatcher.hpp"
#include "CmdNick.hpp"
#include "CmdUser.hpp"
#include "CmdJoin.hpp"

CommandDispatcher::CommandDispatcher()
{
    _commands["NICK"] = new CmdNick();
    _commands["USER"] = new CmdUser();
    _commands["JOIN"] = new CmdJoin();
}

CommandDispatcher::~CommandDispatcher()
{
    std::map<std::string, Command*>::iterator it;

    it = _commands.begin();
    while (it != _commands.end())
    {
        delete it->second;
        it++;
    }
}

void CommandDispatcher::dispatch(Server& server,
                                 Client& client,
                                 const ParsedCommand& cmd)
{
    std::map<std::string, Command*>::iterator it;

    if (cmd.command.empty())
        return;

    it = _commands.find(cmd.command);
    if (it == _commands.end())
        return;

    it->second->execute(server, client, cmd);
}
