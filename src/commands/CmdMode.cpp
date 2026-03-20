/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 18:57:10 by mring             #+#    #+#             */
/*   Updated: 2026/03/16 11:41:30 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../../includes/ircserv.hpp"

static bool parameterCheck(const ParsedCommand &cmd)
{
    // if we have less than 2 parameters (MODE #channel) no flag -> true
    if (cmd.params.size() < 2 || cmd.params[1].size() < 2)
        return true; // true = not enough parameters
    // if [1][1] is +-o OR +k OR +l and [2] is empty -> true
    if ((cmd.params[1][1] == 'o' || ((cmd.params[1][1] == 'k' || cmd.params[1][1] == 'l') && cmd.params[1][0] == '+')) && cmd.params.size() < 3)
        return true;
    return false; // enough parameters
}

// TODO: implement MODE according to RFC 1459
// necessary modes to implement:
// i, t, k, o, l
// i - invite-only channel flag;
// t - topic settable by channel operator only flag;
// k - set a channel key (password).
// o - give/take channel operator privileges;
// l - set the user limit to channel;
void CmdMode::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
{
    // TODO
    // check if nc "MODE +i" with missing channel needs to return anything or is perfectly fine as silent error.
    if (cmd.params.empty())
    {   // ERR_NEEDMOREPARAMS
        clientUser.get_outputBuffer().append(
            ":server 461 " + clientUser.getNickname() + " MODE :Not enough parameters\r\n");
        return;
    }
    // user mode catch 
    // not required by 42 subject, ignoring silently
    if (cmd.params[0][0] != '#')
        return;
    // check if the channel even exists
    if (!server.channelExists(cmd.params[0]))
    {   // ERR_NOSUCHCHANNEL
        clientUser.get_outputBuffer().append(
            ":server 403 " + clientUser.getNickname() + " " + cmd.params[0] + " MODE :No such channel\r\n");
        return;
    }
    // get channel name
    Channel &ch = server.getChannel(cmd.params[0]);
    // query - no flags given, only channel name // MODE <channel>
    if (cmd.params.size() == 1)
    {
        std::string                 modes = "+";
        if (ch.isInviteOnly())      modes += "i";
        if (ch.getTopicFlag())      modes += "t";
        if (!ch.getKey().empty())   modes += "k";
        if (ch.getUserLimit())      modes += "l";
        clientUser.get_outputBuffer().append(
            ":server 324 " + clientUser.getNickname() + " " + cmd.params[0] + " " + modes + "\r\n");
        return;
    }
    // check if enough parameters are given
    if (parameterCheck(cmd))
    {   // ERR_NEEDMOREPARAMS
        clientUser.get_outputBuffer().append(
            ":server 461 " + clientUser.getNickname() + " MODE :Not enough parameters\r\n");
        return;
    }
    // check if channel has the caller as member
    if (!ch.hasMember(clientUser.get_ClientUser_fd()))
    {   // ERR_NOTONCHANNEL
        clientUser.get_outputBuffer().append(
            ":server 442 " + clientUser.getNickname() + " " + cmd.params[0] + " :You're not on that channel\r\n");
        return;
    }
    // check if caller is op
    if (!ch.isOperator(clientUser.get_ClientUser_fd()))
    {   // ERR_CHANOPRIVSNEEDED
        clientUser.get_outputBuffer().append(
            ":server 482 " + clientUser.getNickname() + " " + cmd.params[0] + " :You're not channel operator\r\n");
        return;
    }

    // time to handle the flag:
    // +i / -i -> bool
    // +t / -t -> bool
    // +l / -l -> unsigned int (we would never read int_max, param parsing)
    // +k / -k -> string (parsing)
    // +o / -o -> lookup (nick)

    char sign = cmd.params[1][0];
    char flag = cmd.params[1][1];
    // 
    switch (flag)
    {
        default:
            clientUser.get_outputBuffer().append(
                ":server 472 " + clientUser.getNickname() + " " + flag + " :Unknown MODE flag\r\n");
            break;
        case 'i':
            ch.setInviteOnly(sign);
            break;
        case 't':
            ch.setTopicFlag(sign);
            break;
        case 'l':
            if (sign == '+')
                ch.setUserLimit(cmd.params[2]);
            else if (sign == '-')
                ch.setUserLimit("");
            break;
        case 'k':
            
            if (sign == '+')
            {
                if (!ch.getKey().empty())
                {
                    // 467 ERR_KEYSET
                    clientUser.get_outputBuffer().append(
                        ":server 467 " + clientUser.getNickname() + " " + cmd.params[0] +
                        " :Channel key already set\r\n");
                    return;
                }           
                ch.setKey(cmd.params[2]);
            }
            else if (sign == '-')
                ch.setKey("");
            break;
        case 'o':
        {
            ClientUser* recipient = server.getClientByNick(cmd.params[2]);
            if (!recipient)
            {
                // 401 ERR_NOSUCHNICK
                clientUser.get_outputBuffer().append(
                    ":server 401 " + clientUser.getNickname() + " " + cmd.params[2] +
                    " :No such nick\r\n");
                return;
            }
            if (sign == '+')
                ch.setOperator(recipient->get_ClientUser_fd());
            else if (sign == '-')
                ch.unsetOperator(recipient->get_ClientUser_fd());
            break;
        }
    }
}

/*
461 ERR_NEEDMOREPARAMS
472 ERR_UNKNOWNMODE
482 ERR_CHANOPRIVSNEEDED

324 RPL_CHANNELMODEIS	
*/

// USER MODES (UMODES) are not mandatory and thus not handled.

/*
4.2.3 Mode message

      Command: MODE

   The MODE command is a dual-purpose command in IRC.  It allows both
   usernames and channels to have their mode changed.  The rationale for
   this choice is that one day nicknames will be obsolete and the
   equivalent property will be the channel.

   When parsing MODE messages, it is recommended that the entire message
   be parsed first and then the changes which resulted then passed on.

4.2.3.1 Channel modes

   Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]
               [<ban mask>]

   The MODE command is provided so that channel operators may change the
   characteristics of `their' channel.  It is also required that servers
   be able to change channel modes so that channel operators may be
   created.

   The various modes available for channels are as follows:

         > o - give/take channel operator privileges;
         > i - invite-only channel flag;
         > t - topic settable by channel operator only flag;
         > l - set the user limit to channel;
         > k - set a channel key (password).
           p - private channel flag;
           s - secret channel flag;
           n - no messages to channel from clients on the outside;
           m - moderated channel;
           b - set a ban mask to keep users out;
           v - give/take the ability to speak on a moderated channel;

   When using the 'o' and 'b' options, a restriction on a total of three
   per mode command has been imposed.  That is, any combination of 'o'
   and

   Examples:
           Use of Channel Modes:

MODE #Finnish +i                ; Makes #Finnish channel 'invite-only'.

MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on
                                channel #Finnish.

MODE #42 +k oulu                ; Set the channel key to "oulu".

MODE #42 +t                     ; Only operator can change channel topic now.

MODE #eu-opers +l 10            ; Set the limit for the number of users
                                on channel to 10.
*/
