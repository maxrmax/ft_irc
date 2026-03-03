/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPass.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 20:50:35 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/24 10:33:43 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//CmdPass.hpp
#pragma once

#include "command.hpp"

class CmdPass :public Command
{
    public:
        void execute(Server &server,
                        ClientUser &clientUser,
                        const ParsedCommand &cmd);
};