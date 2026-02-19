/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientUser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 20:06:12 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/17 18:14:03 by nsloniow         ###   ########.fr       */
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
        // use class InputBuffer to make it like a conveyor belt. 
        // Append recv() raw bytes
        InputBuffer     inputBuffer;
        // cut of front bytes on finding \r\n
        OutputBuffer    outputBuffer;
        
        std::string nick_name;
        std::string user_name;
        std::string nickname;
        std::string username;
        std::string realname;
        
        bool        passAccepted;
        bool        registered;
        
  
        public:
            ~ClientUser();
            ClientUser();
            ClientUser(int filedescriptor);
            ClientUser(const ClientUser&) = default;
            ClientUser& operator=(const ClientUser&) = default;

            void set_ClientUser_fd(int filedescriptor);
            int  get_ClientUser_fd();

            InputBuffer     &get_inputBuffer();
            OutputBuffer    &get_outputBuffer();

            // Identity
            void setNickname(const std::string& nick);
            void setUsername(const std::string& user);
            void setRealname(const std::string& real);

            // void getNickname(std::string& nick) const;
            // void getUsername(std::string& user) const;
            std::string getNickname() const;
            std::string getUsername() const;
            //std::string getRealname() const;
            bool    hasNick() const;

            // Auth state
            void setPassAccepted(bool accepted);
            // bool getPassAccepted() const;
            bool isPassAccepted() const;

            void setRegistered(bool reg);
            // bool getRegistered() const;
            bool isRegistered() const;
            bool isReadyToRegister() const;
};