/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdQuit.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 14:59:45 by mring             #+#    #+#             */
/*   Updated: 2026/03/16 18:32:50 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CmdQuit.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

void CmdQuit::execute(Server &server, ClientUser &clientUser, const ParsedCommand &cmd)
{
    std::string reason = clientUser.getNickname();
    if (cmd.params.size() > 0 && !cmd.params[0].empty())
        reason = cmd.params[0];
    std::string quitMsg = ":" + clientUser.getNickname() + 
                            "!" + clientUser.getUsername() +
                            "@ircserver" + " QUIT :" + reason + "\r\n";
    ///////////////////////////

    const int quitterFd = clientUser.get_ClientUser_fd();
    std::vector<std::string> channelList = server.getChannelsOfClientFd(quitterFd);
    std::set<int> recipientList;
    
    for (const auto& channelName : channelList)
    {
        Channel& channel = server.getChannel(channelName);
        const std::set<int>& members = channel.getMembers();
        recipientList.insert(members.begin(), members.end());
    }
    recipientList.erase(quitterFd);
    for (const int recipientFd : recipientList)
    {
        ClientUser* recipient = server.getClientByFd(recipientFd);
        if (recipient)
            recipient->get_outputBuffer().append(quitMsg);
    }
    server.unregisterClientFd(quitterFd);

    // TODO:
    // handle disconnect client in runServer.cpp (currently not disconnecting)
    // and recv() == 0 / read == 0
}

// — (QUIT message is broadcast)	

/*
// 4.1.6 Quit

//       Command: QUIT
//    Parameters: [<Quit message>]

//    A client session is ended with a quit message.  The server must close
//    the connection to a client which sends a QUIT message. If a "Quit
//    Message" is given, this will be sent instead of the default message,
//    the nickname.

//    If, for some other reason, a client connection is closed without  the
//    client  issuing  a  QUIT  command  (e.g.  client  dies and EOF occurs
//    on socket), the server is required to fill in the quit  message  with
//    some sort  of  message  reflecting the nature of the event which
//    caused it to happen.

//    Numeric Replies:

//            None.

//    Examples:

//    QUIT :Gone to have lunch        ; Preferred message format.
*/
