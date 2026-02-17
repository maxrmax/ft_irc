/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdCap copy.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 12:55:40 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/17 12:55:40 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//CmdCap.cpp

#include "../../includes/CmdCap.hpp"
#include "../../includes/ClientUser.hpp"

//LiSt which CAPabilities do you support
void CmdCap::execute(Server&, ClientUser& clientUser, const ParsedCommand& cmd)
{
    // std::cout << "caps\n" ;
    if (!cmd.params.empty() && cmd.params[0] == "LS")
    {
        //:
        //server 
        //CAP was command
        //* to all clients
        //LiSt
        //:start of trailing
        //is empty as we do not support anything
        std::string outputMsg = ":server CAP * LS :\r\n";
        clientUser.get_outputBuffer().append(outputMsg);
    }
};
