/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 18:57:10 by mring             #+#    #+#             */
/*   Updated: 2026/03/09 18:57:10 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cmdMode.hpp"

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
    // When parsing MODE messages, it is recommended that the entire message
    // be parsed first and then the changes which resulted then passed on.

    // needed args:
    // server._channel // unordered_map, learn to check it.
    // 

    /*
    first check set flags:
    +i for invite permissions (else drop silently)
    +t needs chanop permission to change topic
    
    // check if ^ is even needed or if we just jump into op check

    after that: check per use case:
    if op:
    +l [N]/-l
    +o/-o nickname
    +k <passwd>/-k
    +t/-t
    +i/-i

    else
        return (error?);
    */

}

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