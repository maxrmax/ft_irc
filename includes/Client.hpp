/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 20:06:12 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/31 22:03:45 by nsloniow         ###   ########.fr       */
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
};