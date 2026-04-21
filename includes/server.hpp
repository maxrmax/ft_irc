/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:05:52 by nsloniow          #+#    #+#             */
/*   Updated: 2026/04/21 18:30:30 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

#include "poll.hpp"                 // <poll.h>   - <vector>
#include "commandDispatcher.hpp"    // <map>      - <string>
#include "Channel.hpp"              // <set>      - <string> - <vector> - <unordered_set>
#include "ClientUser.hpp"           // <string>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * mring:
 *      it is a global variable (const int has internal linkage, implicit static)
 *      it still is a global variable, after the change, just optimized
 *      extern is used inside a file that tries to access this variables
 * Changes:
 *      const int to inline constexpr int
 * constexpr:
 *      compile-time evaluation, stronger guarantee than const
 * inline:
 *      one shared instance across all translation units
 *      without inline each .cpp that includes this header would get a copy
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
inline constexpr int    ADDRESS_FAMILY  =   AF_INET;
inline constexpr int    PORT_LISTEN     =   6667;

// Channel and ClientUser definitions are included above

/**
 * Server: core IRC server container
 *
 * - owns the listening socket info (server_fd, server_address)
 * - owns maps for clients, channels and nickname lookups
 * - provides channel-management and client-registry APIs used by command handlers
 *
 * Conventions:
 * - Methods returning references (Channel&) refer into the _channels map.
 *   Caller must not store references across operations that may mutate the map.
 * - getChannel(name) uses .at() and will throw std::out_of_range if missing.
 * - registerClientFd/unregisterClientFd manage the _clients map ownership (ClientUser* stored).
 */
class Server
{
    private:
        // listening socket fd
        int                                             server_fd;
        // configured port
        int                                             server_port;
        // server password (PASS)
        std::string                                     server_password;
        // listening address struct
        sockaddr_in                                     server_address;
        // dispatches IRC commands to handlers
        CommandDispatcher                               dispatcher;
        
        // array for fd to poll, to check, if ready with data
        std::vector<pollfd>                             poll_fd;
        // mapping for (unique index) fd to Client object 
        // unordered map jumps to item by index and is faster than pure std::map.
        // int is the index which is the fd (or client_accepted_fd on assignment)
        // ClientUser is our object we map by fd
        // populate via registerClientFd() and remove via unregisterClientFd()
        std::unordered_map<int, ClientUser>             _clients;

        std::unordered_map<std::string, ClientUser*>    nicknames_history;
        // current nickname -> ClientUser* for O(1) lookup
        std::unordered_map<std::string, ClientUser*>    nick_clientUser;
        // channel name -> Channel object (owned by server)
        // Channel references returned by createChannel/getChannel point into this map
        std::unordered_map<std::string, Channel>        _channels;


    public:
        ~Server();
        Server();
        Server(int filedescriptor, int port, std::string password);

        int                                         get_server_fd();
        std::string                                 get_server_password();
        sockaddr_in                                 get_server_address();
        std::vector<pollfd>                         &getPollFD();
        const std::vector<pollfd>                   &getPollFD() const;
        std::unordered_map<int, ClientUser>         &get_clients_map();
        const std::unordered_map<int, ClientUser>   &get_clients_map() const;
        void                                        setClientIp(int fd, const std::string &str_ip);
        const std::string                           getClientIp(int fd);

        void                                        printRegisteredNicks();
        
        int                                         get_server_ready(int portt, std::string password);

        CommandDispatcher                           &get_dispatcher();
        bool                                        NickIsAlreadyRegistered(std::string nick) const;
        void                                        NicknamesHistory_storing(std::string previouseNickname, ClientUser &clientUser);
        void                                        Nick_ClientUser_mapping(ClientUser &clientUser);

        /* Channel management API
         * - getChannelsOfClientFd: returns vector of channel names the fd is member of
         * - channelExists: fast check for presence in _channels
         * - createChannel: create if missing and return reference to stored Channel
         * - getChannel: access existing Channel by name (uses .at(), may throw)
         * - removeChannel: erase channel from _channels
         * - broadcastToChannel / broadcastToChannelExcept: append raw msg to member output buffers
         * - getChannelMemberNicks: build space-separated nick list; '@' prefixed for ops
         */
        std::vector<std::string>    getChannelsOfClientFd(int fd);
        bool                        channelExists(const std::string& name) const;
        Channel&                    createChannel(const std::string& name, ClientUser& founder);
        Channel&                    getChannel(const std::string& name);
        void                        removeChannel(const std::string& name);
        void                        broadcastToChannel(const std::string& channelName, const std::string& msg);
        void                        broadcastToChannelExcept(const std::string& channelName, const std::string& msg, int excludeFd);
        std::string                 getChannelMemberNicks(const std::string& channelName) const;

        /* Client fd registry
         * - registerClientFd stores a pointer in _clients for routing
         * - unregisterClientFd removes mappings, clears nick lookup and removes empty channels
         * Ownership note: Server stores raw ClientUser*; lifecycle (allocation/deletion) is managed elsewhere.
         */
        void                        registerClientFd(int fd);
        void                        unregisterClientFd(int fd);

        // lookup helpers (return nullptr if not found)
        ClientUser*                 getClientByNick(const std::string& nick);
        ClientUser*                 getClientByFd(int fd);
};
