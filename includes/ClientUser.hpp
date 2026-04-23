/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientUser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 20:06:12 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/16 11:47:07 by mring            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "inputBuffer.hpp"  // <string>
#include "OutputBuffer.hpp" // <string

class Server;
struct ParsedCommand;

/**
 * ClientUser
 *
 * Represents a connected IRC client:
 * - owns per-client input/output buffers and identity strings (nick, user, realname, ip)
 * - stores connection state flags (registered, passAccepted, toDisconnect)
 *
 * Ownership / lifetime:
 * - Instances are referenced from Server via raw ClientUser* (non-owning). The component
 *   that creates the ClientUser (accept loop / connection manager) must manage destruction.
 * - Server/registerClientFd stores pointers for routing; unregisterClientFd must be called
 *   before the owner destroys the object to avoid dangling pointers.
 */
class ClientUser
{
private:
    int             ClientUser_fd;      // socket file descriptor for this client
    InputBuffer     inputBuffer;        // owned; stores incoming parsed/raw data
    OutputBuffer    outputBuffer;       // owned; stores outgoing messages to send
    
    std::string     nickname;           // current nick (empty if not set)
    std::string     username;           // USER field
    std::string     realname;           // realname / GECOS
    std::string     ip;                 // remote IP address
    
    bool            registered;         // true after full registration (NICK+USER)
    bool            passAccepted;       // PASS accepted for servers requiring a password
    bool            toDisconnect;       // flag indicating connection should be closed

public:
    ~ClientUser();
    ClientUser();
    ClientUser(int filedescriptor);
    ClientUser(const ClientUser&) = default;
    ClientUser& operator=(const ClientUser&) = default;

    void            set_ClientUser_fd(int filedescriptor);
    int             get_ClientUser_fd();

    // Buffers: return references to owned buffers (caller must NOT delete)
    InputBuffer     &get_inputBuffer();
    OutputBuffer    &get_outputBuffer();

    void            setNickname(const std::string& nick);
    void            setUsername(const std::string& user);
    void            setRealname(const std::string& real);
    void            setIp(const std::string& host);

    std::string     getNickname() const;
    std::string     getUsername() const;
    std::string     getRealname() const;
    std::string     getIp() const;

    bool            hasNick() const;

    void            setPassAccepted(bool accepted);
    bool            isPassAccepted() const;

    void            setRegistered(bool reg);
    bool            isRegistered() const;

    bool            isReadyToRegister() const;

    void            setToDisconnect(bool disconnect);
    bool            isToDisconnect() const;
};