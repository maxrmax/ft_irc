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

struct ParsedCommand
{
    std::string command;
    std::vector<std::string> params;
};

class Parser
{
public:
    static ParsedCommand parseLine(const std::string& rawInp);

private:
    // Helper functions
    static std::string trim(const std::string& str);
    static bool isValidCommand(const std::string& cmd);
    static std::string toUpper(const std::string& str);
    static void skipSpaces(const std::string& line, size_t& i);
    static std::string parseToken(const std::string& line, size_t& i);
};

#endif // PARSER_HPP
