/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandDispatcher.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:07:05 by ngoyat            #+#    #+#             */
/*   Updated: 2026/01/30 12:07:05 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

class Command;
class Server;
class ClientUser;
struct ParsedCommand;

/**
 * CommandDispatcher
 *
 * Responsible for mapping IRC command names to `Command` handlers and invoking
 * the appropriate handler for a parsed input.
 *
 * Notes:
 * - `_commands` stores pointers to `Command` instances indexed by command name.
 * - Lifetime/ownership: the implementation should document whether the
 *   dispatcher owns the `Command*` instances (ctor allocates, dtor deletes)
 *   or if they are managed elsewhere. The destructor is the correct place to
 *   clean up owned pointers.
 */
class CommandDispatcher {
public:
    CommandDispatcher();
    ~CommandDispatcher();

    void dispatch(Server& server,
                  ClientUser& client,
                  const ParsedCommand& cmd);

private:
    std::map<std::string, Command*> _commands;
};
