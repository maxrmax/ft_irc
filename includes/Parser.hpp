#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

struct ParsedCommand {
    std::string command;
    std::vector<std::string> params;
};

class Parser {
public:
    static ParsedCommand parseLine(const std::string& line);

private:
    static std::string trim(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool        isValidCommand(const std::string& cmd);
    static void        skipSpaces(const std::string& line, size_t& i);
    static std::string parseToken(const std::string& line, size_t& i);
};

#endif