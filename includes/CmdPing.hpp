/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdPing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 18:57:34 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/09 19:06:45 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//CmdPing.hpp

#pragma once
#include "command.hpp"

class CmdPing : public Command 
{
    public:
        void execute(Server &server,
                     ClientUser &client,
                     const ParsedCommand &cmd) override;
};
