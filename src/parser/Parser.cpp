/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:04:38 by ngoyat            #+#    #+#             */
/*   Updated: 2026/01/30 12:04:38 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Parser.hpp"
#include <cctype>
#include "../../includes/ircserv.hpp"

ParsedCommand Parser::parseLine(const std::string& rawInp)
{
    std::cout << __LINE__ << ": \n" << std::endl;

    ParsedCommand cmd;
    std::string line;
    size_t i;

    line = trim(rawInp);
    if (line.empty())
        return cmd;

    i = 0;
    skipSpaces(line, i);
    cmd.command = toUpper(parseToken(line, i));

    if (cmd.command.empty() || isValidCommand(cmd.command) == false)
    {
        cmd.command.clear();
        return cmd;
    }

    skipSpaces(line, i);
    while (i < line.length())
    {
        if (line[i] == ':')
        {
            cmd.params.push_back(line.substr(i + 1));
            break;
        }
        cmd.params.push_back(parseToken(line, i));
        skipSpaces(line, i);
    }
    return cmd;
}

// Helper functions

std::string Parser::trim(const std::string& str)
{
    size_t start;
    size_t end;

    if (str.empty())
        return "";

    start = 0;
    while (start < str.length() && std::isspace(str[start]))
        start++;

    if (start == str.length())
        return "";

    end = str.length() - 1;
    while (end > start && std::isspace(str[end]))
        end--;

    return str.substr(start, end - start + 1);
}

bool Parser::isValidCommand(const std::string& cmd)
{
    size_t i;

    i = 0;
    while (i < cmd.length())
    {
        if (std::isalnum(cmd[i]) == false)
            return false;
        i++;
    }
    return true;
}

std::string Parser::toUpper(const std::string& str)
{
    std::string res;
    size_t i;

    i = 0;
    while (i < str.length())
    {
        res += std::toupper(str[i]);
        i++;
    }
    return res;
}

void Parser::skipSpaces(const std::string& line, size_t& i)
{
    while (i < line.length() && line[i] == ' ')
        i++;
}

std::string Parser::parseToken(const std::string& line, size_t& i)
{
    size_t start;

    start = i;
    while (i < line.length() && line[i] != ' ')
        i++;
    return line.substr(start, i - start);
}
