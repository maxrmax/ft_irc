#ifndef COMMAND_HPP
#define COMMAND_HPP
#include "Parser.hpp"

class Server;
class Client;

class Command {
public:
    virtual ~Command() {}
    virtual void execute(Server& server,
                         Client& client,
                         const ParsedCommand& cmd) = 0;
};
#endif // COMMAND_HPP