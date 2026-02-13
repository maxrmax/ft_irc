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

#ifndef COMMAND_HPP
#define COMMAND_HPP
#include "Parser.hpp"

class Server;
class ClientUser;

class Command {
public:
    virtual ~Command() {}
    virtual void execute(Server& server,
                         ClientUser& client,
                         const ParsedCommand& cmd) = 0;
};
#endif // COMMAND_HPP