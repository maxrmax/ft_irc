#ifndef COMMANDDISPATCHER_HPP
#define COMMANDDISPATCHER_HPP
#include <map>
#include <string>
#include "command.hpp"

class CommandDispatcher {
public:
    CommandDispatcher();
    ~CommandDispatcher();

    void dispatch(Server& server,
                  Client& client,
                  const ParsedCommand& cmd);

private:
    std::map<std::string, Command*> _commands;
};
#endif // COMMANDDISPATCHER_HPP