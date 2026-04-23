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

#include "CmdCap.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

// List which Capabilities the Server supports when irssi clients connect
void CmdCap::execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    (void)server; // unused parameter

    if (!cmd.params.empty() && cmd.params[0] == "LS")
    {
        std::string outputMsg = ":server CAP * LS :";
        clientUser.get_outputBuffer().append(outputMsg);
    }
};
