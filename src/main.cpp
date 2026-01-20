#include <iostream>
#include "Parser.hpp"

static void printCommand(const ParsedCommand& cmd)
{
    size_t i;

    if (cmd.command.empty())
    {
        std::cout << "[invalid]\n";
        return;
    }

    std::cout << "Command: [" << cmd.command << "]\n";
    std::cout << "Params:\n";

    i = 0;
    while (i < cmd.params.size())
    {
        std::cout << "  [" << i << "] -> [" << cmd.params[i] << "]\n";
        i++;
    }
    std::cout << "----------------------\n";
}

int main(void)
{
    std::string line;
    ParsedCommand cmd;

    std::cout << "IRC Parser Test (Ctrl+D to quit)\n";
    while (std::getline(std::cin, line))
    {
        cmd = Parser::parseLine(line);
        printCommand(cmd);
    }
    return 0;
}
