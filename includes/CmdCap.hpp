/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdCap.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 12:52:57 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/17 15:14:39 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//CcmdCap.hpp

#pragma once
#include "command.hpp"

class CmdCap : public Command 
{
    public:
        void execute(Server& server, ClientUser& client, const ParsedCommand& cmd);
};