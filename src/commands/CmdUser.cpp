/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdUser.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 01:22:33 by ngoyat            #+#    #+#             */
/*   Updated: 2026/04/23 11:05:06 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// CmdUser.cpp
#include "CmdUser.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

// IRC USER syntax: USER <username> <hostname> <servername> :<realname>
void CmdUser::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
{
    if (clientUser.isRegistered())
    {
        clientUser.get_outputBuffer().append(
            ":server 462 :You may not reregister");
        return;
    }

    if (cmd.params.size() < 4)
    {
        clientUser.get_outputBuffer().append(
            ":server 461 USER :Not enough parameters");
        return;
    }

    std::string username = cmd.params[0];
    std::string realname = cmd.params[3];

    clientUser.setUsername(username);
    clientUser.setRealname(realname);

    // Check if ready to register after setting nickname and username
    // USER firstname hostname servername :Real Name
    // (: means "rest of line", its defined in parser.cpp)
    if (clientUser.isReadyToRegister() && !clientUser.isRegistered())
    {
        clientUser.setRegistered(true);

        server.Nick_ClientUser_mapping(clientUser);

        server.printRegisteredNicks();

        clientUser.get_outputBuffer().append(
            ":server 001 " + clientUser.getNickname() +
            " :Welcome to ircserver " +
            clientUser.getNickname() + "!" +
            clientUser.getUsername() + "@ircserver");
    }
}

/*
461 ERR_NEEDMOREPARAMS
462 ERR_ALREADYREGISTRED
*/

// hostname and servername are depracated
// legacy - we support the input but we don't do anything with it.

/*
// 4.1.3 User message

//       Command: USER
//    Parameters: <username> <hostname> <servername> <realname>

//    The USER message is used at the beginning of connection to specify
//    the username, hostname, servername and realname of s new user.  It is
//    also used in communication between servers to indicate new user
//    arriving on IRC, since only after both USER and NICK have been
//    received from a client does a user become registered.

//    Between servers USER must to be prefixed with client's NICKname.
//    Note that hostname and servername are normally ignored by the IRC
//    server when the USER command comes from a directly connected client
//    (for security reasons), but they are used in server to server
//    communication.  This means that a NICK must always be sent to a
//    remote server when a new user is being introduced to the rest of the
//    network before the accompanying USER is sent.

//    It must be noted that realname parameter must be the last parameter,
//    because it may contain space characters and must be prefixed with a
//    colon (':') to make sure this is recognised as such.

//    Since it is easy for a client to lie about its username by relying
//    solely on the USER message, the use of an "Identity Server" is
//    recommended.  If the host which a user connects from has such a
//    server enabled the username is set to that as in the reply from the
//    "Identity Server".

//    Numeric Replies:

//            ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED

//    Examples:


//    USER guest tolmoon tolsun :Ronnie Reagan
//                                    ; User registering themselves with a
//                                    username of "guest" and real name
//                                    "Ronnie Reagan".


//    :testnick USER guest tolmoon tolsun :Ronnie Reagan
//                                    ; message between servers with the
//                                    nickname for which the USER command
//                                    belongs to
*/