/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:10:50 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/15 17:15:11 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <iostream>

#include "server.hpp" // <fcntl.h> - <iostream> - <netinet/in.h> - <cstring> - <sys/types.h> - <sys/socket.h> - <unistd.h> - <unordered_map>
/* server.hpp:
"poll.hpp"                 // <poll.h>   - <vector>
"commandDispatcher.hpp"    // <map>      - <string>
"Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
"ClientUser.hpp"           // <string>
*/
#include "Parser.hpp"   // <string> - <vector>
#include "checker.hpp"  // <string>

// forward declaration from runServer.cpp
void runServer(Server &irc_server);

//volatile:
// tells the compiler this variable can be changed anytime
// without this the compiler could optimize while (!g_shutdown) to while (true)
//sig_atomic_t:
// Guaranteed atomic read/write.
// A normal int could theoretically be written byte by byte
// if the signal interferes, the loop reads a partially written value.
volatile sig_atomic_t g_shutdown = 0;

static void signal_handler(int signum)
{
    (void)signum;   // wir brauchen den Signalnamen nicht – alle machen dasselbe
    g_shutdown = 1;
}

static void setup_signals(void)
{
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;            // kein SA_RESTART!

    sigaction(SIGINT,  &sa, NULL);  // Ctrl+C
    sigaction(SIGTERM, &sa, NULL);  // kill <pid> / systemd stop

    // SIGTSTP = Ctrl+Z (Terminal Stop)
    // Wir ignorieren es, damit der Server nicht einfach im Hintergrund
    // hängt und Clients verbunden bleiben
    // SIGTSTP wird meistens von der shell abgefangen
    signal(SIGTSTP, SIG_IGN);

    // SIGPIPE: Tritt auf wenn wir send() an einen Client aufrufen,
    // der seine Verbindung bereits geschlossen hat.
    // Standardverhalten: Prozess wird sofort beendet (crash!).
    // Mit SIG_IGN: send() gibt stattdessen -1 mit errno = EPIPE zurück,
    // was wir normal behandeln können.
    signal(SIGPIPE, SIG_IGN);

    // Ctrl+D sendet EOF auf stdin. 
    // Da unser Server keine stdin liest,
    // passiert einfach gar nichts kein Handling nötig.
}

void printCommand(const ParsedCommand& cmd, const ClientUser& clientUser)
{
    size_t i;

    if (cmd.command.empty())
    {
        std::cout << "[empty command]\n";
        return;
    }

    std::cout << "User:    [" << clientUser.getUsername() << "]\n";
    std::cout << "Command: [" << cmd.command << "]\n";
    std::cout << "Params:\n";

    i = 0;
    while (i < cmd.params.size())
    {
        std::cout << "  [" << i << "] -> [" << cmd.params[i] << "]\n";
        i++;
    }
    std::cout << "----------------------\n";
}

int check_arguments(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Run like : ./ircserv <port> <connection password>" << std::endl;
        return (-1);
    }
    if (!is_digits_only(argv[1]) || std::stoi(argv[1]) < 1028)
    {
        std::cout << "Run like : ./ircserv <port> <connection password>" << std::endl;
        std::cout << "Port needs to be a number above 1027." << std::endl;
        return (-1);
    }
    return (0);
}

int main(int argc, char **argv)
{
    if (check_arguments(argc, argv) == -1)
        return (-1);

    setup_signals();

    Server  irc_server;
    if (irc_server.get_server_ready(std::stoi(argv[1]), argv[2]) == -1)
        return -1;
    std::cout << "Server created. fd = " << irc_server.get_server_fd() << std::endl;

    runServer(irc_server);

    return (0);
}
