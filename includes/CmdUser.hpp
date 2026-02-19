/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdUser.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 01:29:04 by ngoyat            #+#    #+#             */
/*   Updated: 2026/02/19 01:34:42 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CMDUSER_HPP
#define CMDUSER_HPP

#include "command.hpp"

class CmdUser : public Command
{
public:
    void execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd);
};

#endif
