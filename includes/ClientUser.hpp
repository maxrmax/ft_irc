/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientUserUser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 20:06:12 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/12 22:14:01 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//ClientUser.hpp
#pragma once

// #include <iostream>
// #include <unistd.h>
#include "command.hpp"
#include "inputBuffer.hpp"
#include "OutputBuffer.hpp"
#include "ircserv.hpp"
#include "Parser.hpp"

class ClientUser
{
    private:
        int         ClientUser_fd;
        std::string nick_name;
        std::string user_name;
        // ParsedCommand   


        // std::string message_received;
        // std::string message_put_together;
        // use class InputBuffer to make it like a conveyor belt. 
        // Append recv() raw bytes
        // cut of front bytes on finding \r\n
        InputBuffer inputBuffer;
        OutputBuffer outputBuffer;
  
        public:
            ~ClientUser();
            ClientUser();
            ClientUser(int filedescriptor);

            void set_ClientUser_fd(int filedescriptor);
            // void set_message_received(std::string message_received);
            int  get_ClientUser_fd();
            // std::string get_message_received();
            // std::string get_message_put_together();
            InputBuffer &get_inputBuffer();
            OutputBuffer &get_outputBuffer();
            
            void put_message_together();
};