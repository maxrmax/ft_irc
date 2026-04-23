/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Jarvis.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 18:37:39 by ngoyat            #+#    #+#             */
/*   Updated: 2026/04/23 11:05:06 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Triggered when a PRIVMSG to a channel starts with "@jarvis"
// Sends a NOTICE back to the triggering user only (not the channel).
//
// Usage examples:
//   @jarvis time        → current timestamp
//   @jarvis             → help text
 
// Standalone IRC command - no hook in CmdPrivmsg.
// User activates with: /jarvis <arg>
// irssi sends: JARVIS <arg>
//
// Responds with a NOTICE to the calling user.
 
// Two ways to trigger:
//   1. /quote JARVIS time        (outside channel, raw command)
//   2. @jarvis time              (inside channel as PRIVMSG text)
//
// CmdPrivmsg calls CmdJarvis::handleMessage() when it detects "@jarvis".
// No Jarvis logic lives in CmdPrivmsg – only a one-line check and delegation.
 
#include <ctime>
#include "Jarvis.hpp"
#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/

static const std::string JARVIS_PREFIX = ":Jarvis!bot@ircserver";

static const std::string FACTS[] = {
    "At its peak in 2003, IRC had over 10 million users simultaneously connected.",
    "The first computer bug was an actual bug – a moth found in a Harvard Mark II relay in 1947.",
    "There are only 10 types of people in the world: those who understand binary and those who don't.",
    "Python is named after Monty Python, not the snake.",
    "The QWERTY keyboard layout was designed to slow typists down to prevent typewriter jams.",
    "Unix time started on January 1, 1970 – also known as the Unix epoch.",
    "In C, undefined behavior means literally anything can happen – including your code appearing to work.",
    "A googol (10^100) is less than a googolplex (10^googol), which cannot be written out even if every atom in the universe were a digit.",
    "Light takes about 8 minutes to travel from the Sun to Earth – so you're seeing the Sun as it was 8 minutes ago.",
    "The 'ping' command is named after the sound of sonar – it sends a packet and listens for the echo.",
    "HTTP 418 'I'm a teapot' is a real status code defined in RFC 2324 as an April Fools' joke in 1998.",
    "The 'floppy' in floppy disk refers to the flexibility of the original 8-inch magnetic storage medium.",
    "TCP/IP was designed to survive a nuclear war – packets are routed around damage automatically.",
    "A race condition, a memory leak, and an off-by-one error walk into a bar. Sometimes.",
    "The first computer programmer was Ada Lovelace, who wrote an algorithm for Charles Babbage's Analytical Engine in 1843.",
};
static const size_t FACTS_COUNT = sizeof(FACTS) / sizeof(FACTS[0]);

static void jarvisNotice(ClientUser& target, const std::string& noticeTarget, const std::string& text)
{
    std::string msg = JARVIS_PREFIX + " NOTICE " + noticeTarget + " :" + text;
    target.get_outputBuffer().append(msg);
 
    const std::string& buf = target.get_outputBuffer().get_buffer();
    if (!buf.empty())
    {
        ssize_t sent = send(target.get_ClientUser_fd(), buf.c_str(), buf.size(), 0);
        if (sent > 0)
            target.get_outputBuffer().get_buffer().erase(0, sent);
    }
}

static std::string parseArgFromText(const std::string& text)
{
    size_t pos = 7; // skip "@jarvis"
    while (pos < text.size() && text[pos] == ' ')
        pos++;
    std::string arg = text.substr(pos);
    return arg;
}

static std::string parseArgFromCmd(const ParsedCommand& cmd)
{
    if (cmd.params.empty())
        return "";
    std::string arg = cmd.params[0];
    if (!arg.empty() && arg[0] == ':')
        arg = arg.substr(1);
    return arg;
}

static void dispatch(ClientUser& clientUser, const std::string& noticeTarget, const std::string& arg)
{
        if (arg == "420")
    {
        // IRC formatting: NN = color (09=green),  = italic,  = reset
        std::string msg = "09( ͡° ͜ʖ ͡°) blaze it";
        jarvisNotice(clientUser, noticeTarget, msg);
        return;
    }
    if (arg.empty())
    {
        // Random fact
        size_t index = static_cast<size_t>(std::time(nullptr)) % FACTS_COUNT;
        jarvisNotice(clientUser, noticeTarget, FACTS[index]);
        return;
    }
    if (arg == "time")
    {
        std::time_t now = std::time(nullptr) + 3600; // GMT+1
        char buf[64];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S GMT+1", std::gmtime(&now));
        jarvisNotice(clientUser, noticeTarget, std::string("Current time: ") + buf);
        return;
    }
    if (arg == "help")
    {
        jarvisNotice(clientUser, noticeTarget, "Available commands: time, help. Or just @jarvis for a random fact!");
        return;
    }
    jarvisNotice(clientUser, noticeTarget, "Unknown command '" + arg + "'. Try: @jarvis help");
}

void CmdJarvis::handleMessage(ClientUser& clientUser, const std::string& channel, const std::string& text)
{
    dispatch(clientUser, channel, parseArgFromText(text));
}

void CmdJarvis::execute(Server&, ClientUser& clientUser, const ParsedCommand& cmd)
{
    if (!clientUser.isRegistered())
    {
        clientUser.get_outputBuffer().append(":server 451 * :You have not registered");
        return;
    }
    dispatch(clientUser, clientUser.getNickname(), parseArgFromCmd(cmd));
}
