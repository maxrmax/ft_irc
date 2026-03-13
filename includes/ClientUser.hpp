/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientUser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 20:06:12 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/13 10:56:10 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//ClientUser.hpp
#pragma once

#include "command.hpp"
#include "inputBuffer.hpp"
#include "OutputBuffer.hpp"
#include "ircserv.hpp"
#include "Parser.hpp"

class ClientUser
{
    private:
        int             ClientUser_fd;
        InputBuffer     inputBuffer;
        OutputBuffer    outputBuffer;
        
        std::string     nickname;
        std::string     username;
        std::string     realname;
        std::string     ip;
        
        
        bool            registered;
        bool            passAccepted;
        bool            toDisconnect;
        
  
        public:
            ~ClientUser();
            ClientUser();
            ClientUser(int filedescriptor);
            ClientUser(const ClientUser&) = default;
            ClientUser& operator=(const ClientUser&) = default;

            void            set_ClientUser_fd(int filedescriptor);
            int             get_ClientUser_fd();

            InputBuffer     &get_inputBuffer();
            OutputBuffer    &get_outputBuffer();

            // Identity
            void            setNickname(const std::string& nick);
            void            setUsername(const std::string& user);
            void            setRealname(const std::string& real);
            void            setIp(const std::string& host);

            std::string     getNickname() const;
            std::string     getUsername() const;
            std::string     getRealname() const;
            std::string     getIp() const;
            bool            hasNick() const;

            // Auth state
            void            setPassAccepted(bool accepted);
            bool            isPassAccepted() const;

            void            setRegistered(bool reg);
            bool            isRegistered() const;
            bool            isReadyToRegister() const;

            // WIP TODO
            void            setToDisconnect(bool disconnect);
            bool            isToDisconnect() const;
};