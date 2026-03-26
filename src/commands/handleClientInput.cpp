/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleClientInput.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 12:57:30 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/18 03:57:06 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

// Lightweight declaration of printCommand used for debugging output.
// The full declaration (and other utilities) live in the umbrella header
// `ircserv.hpp`. We declare the function here to avoid including the
// entire umbrella; keep the signature in sync with that declaration.
void printCommand(const ParsedCommand& cmd, const ClientUser& clientUser);

void handleClientInput(ClientUser *clientUser, Server &irc_server)
{
    while (clientUser->get_inputBuffer().hasLine())
    {
        // parse msg for command
        ParsedCommand cmd = Parser::parseLine(clientUser->get_inputBuffer().popLine());
        printCommand(cmd, *clientUser);
        irc_server.get_dispatcher().dispatch(irc_server, *clientUser, cmd);
    }
}