/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdNick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:05:53 by ngoyat            #+#    #+#             */
/*   Updated: 2026/01/30 12:05:53 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cmdNick.hpp"
#include "../../includes/ClientUser.hpp"

//IRC rules - RFC 1459
// Must start with a letter or special character ([A-Za-z\[\]\\_^{|}-]`)
// Can contain letters, digits, and special characters ([A-Za-z0-9\[\]\\_^{|}-]`)
// Maximum length often 9 characters in RFC1459, but modern clients accept more
// No spaces or control characters
static bool NickIsValid(std::string nick)
{
    if (nick.empty())
        return false;
    
    if (nick.size() > 9)
        return false;

    //1st is letter only
    if (!std::isalpha(nick[0]))
        return false;
    
    //check rest of chars
    for (unsigned long i = 1; i < nick.size(); i++)
    {
        if (!std::isdigit(nick[i]) && !std::isalpha(nick[i]) && !isspecial(nick[i]))
            return false;
    } 
    return true;

};

void CmdNick::execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    if (cmd.params.empty())
    {
        //NONICKNAMEGIVEN)
        clientUser.get_outputBuffer().append(":server 431 * " + cmd.command + ":No nickname given\r\n");
        return;
    }
    
    //validate nickname
    if (!NickIsValid(cmd.params[0]))
    {
        //max 9 char
        //no space 9, 32, 
        //hostname of the client
        // std::cout << "nick size: " << cmd.params[0].size() << std::endl;
        // for (size_t i = 0; i < cmd.params[0].size(); i++)
        //     std::cout << (int)cmd.params[0][i] << cmd.params[0][i] << std::endl;

        //ERR_ERRONEUSNICKNAME) 
        clientUser.get_outputBuffer().append(":server 432 * " + cmd.params[0] + " :Erroneus nickname\r\n");
        return;
    }
    
    if (server.NickIsAlreadyRegistered(cmd.params[0]))
    {
        // 433     ERR_NICKNAMEINUSE
        clientUser.get_outputBuffer().append(":server 433 * " + cmd.params[0] + " :Nickname is already in use\r\n");
    }

    //store nickname
    std::string previouseNickname = clientUser.getNickname();
    clientUser.setNickname(cmd.params[0]);
    server.Nicknames_storing(cmd.params[0]);
    
    if (previouseNickname == clientUser.getNickname())
    {
        server.NicknamesHistory_storing(clientUser.getNickname(), previouseNickname);
        
        //nickname changed → broadcast
    //     for (auto &[fd, registered_ClientUser] : server. )
    //     {
    //         if (clientUser == registe)
    //         clientUser.get_outputBuffer().append(":server " + previouseNickname + " NICK : " + clientUser.getNickname());
    //     }        
    }

    // if (clientUser.isReadyToRegister() && !clientUser.getNickname().empty() && !clientUser.getUsername().empty() && clientUser.isRegistered() == false)
    if (clientUser.isReadyToRegister() && !clientUser.isRegistered())
    {
        clientUser.setRegistered(true);
        server.Nick_ClientUser_mapping(clientUser);
        // send welcome message 001
        clientUser.get_outputBuffer().append(":server 001 " + clientUser.getNickname() + 
            " :Welcome to ircserver NICK" + clientUser.getNickname() + "!" + clientUser.getUsername() + "@ircserver\r\n");
        return;
        // ERR_ALREADYREGISTRED
    }
    server.printRegisteredNicks();

// TODO: check for valid nicknames + add error handling + add to client.cpp
    // client.setNick(cmd.params[0]);
}
