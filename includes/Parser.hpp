/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:10:12 by ngoyat            #+#    #+#             */
/*   Updated: 2026/01/30 12:10:12 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

/**
 * ParsedCommand
 *
 * Simple POD representing a parsed IRC command and its parameters.
 * - `command` is the upper-cased command verb (e.g. "PRIVMSG")
 * - `params` contains the parameter tokens in order
 */
struct ParsedCommand
{
    std::string command;
    std::vector<std::string> params;
};

/**
 * Parser
 *
 * Minimal parser for raw IRC input lines. Responsibilities:
 * - validate command tokens
 * - split parameters (including handling the trailing ':' parameter)
 * - normalize command to upper-case for dispatcher lookup
 *
 * Static API: `parseLine` takes a raw input line and returns a `ParsedCommand`.
 */
class Parser
{
public:
    static ParsedCommand parseLine(const std::string& rawInp);

private:
    // Helper functions used by parseLine
    static std::string trim(const std::string& str);
    static bool isValidCommand(const std::string& cmd);
    static std::string toUpper(const std::string& str);
    static void skipSpaces(const std::string& line, size_t& i);
    static std::string parseToken(const std::string& line, size_t& i);
};

#endif // PARSER_HPP
