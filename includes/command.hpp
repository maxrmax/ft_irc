/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:06:54 by ngoyat            #+#    #+#             */
/*   Updated: 2026/01/30 12:06:54 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"

class Server;
class ClientUser;
// struct ParsedCommand;

/**
 * Command
 *
 * Abstract base class for all IRC command handlers.
 * Implementations override `execute` to perform the command's action.
 *
 * Expectations / responsibilities:
 * - `execute` is invoked by the dispatcher with references to the
 *   `Server`, the calling `ClientUser`, and a `ParsedCommand` describing
 *   the verb and parameters.
 * - Handlers should append replies to the client's `OutputBuffer` or
 *   modify server/channel state using Server APIs. They must not delete
 *   the `ClientUser` pointer or otherwise assume ownership of server-managed
 *   objects.
 */
class Command {
public:
    virtual ~Command() {}
    virtual void execute(Server& server,
                         ClientUser& client,
                         const ParsedCommand& cmd) = 0;
};
