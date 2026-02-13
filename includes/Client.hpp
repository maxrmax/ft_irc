/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 20:06:12 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/13 13:35:55 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//client.hpp
#pragma once

// #include <iostream>
// #include <unistd.h>
#include "inputBuffer.hpp"
#include "ircserv.hpp"

class Client
{
    private:
        int         client_fd;
        // std::string message_received;
        // std::string message_put_together;
        // use class InputBuffer to make it like a conveyor belt. 
        // Append recv() raw bytes
        // cut of front bytes on finding \r\n
        std::string nickname;
        std::string username;
        
        bool        passAccepted;
        bool        registered;
        
        InputBuffer inputBuffer;
  
        public:
            ~Client();
            Client();
            Client(int filedescriptor);

            void set_client_fd(int filedescriptor);
            // void set_message_received(std::string message_received);
            int  get_client_fd();
            // std::string get_message_received();
            // std::string get_message_put_together();
            InputBuffer &get_inputBuffer();

            void put_message_together();

            // Identity
            void setNickname(const std::string& nick);
            void setUsername(const std::string& user);

            void getNickname(std::string& nick) const;
            void getUsername(std::string& user) const;

            // Auth state
            void setPassAccepted(bool accepted);
            bool getPassAccepted() const;

            void setRegistered(bool reg);
            bool getRegistered() const;
            bool isReadyToRegister() const;
};