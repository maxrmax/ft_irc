/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 20:06:12 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/27 19:40:49 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//client.hpp
#pragma once

#include <iostream>
#include <unistd.h>

class Client
{
    private:
        int         client_fd;
        // std::string message;
        std::string message_received;
        std::string message_put_together;
  
        public:
            ~Client();
            Client();
            Client(int filedescriptor);

            void set_client_fd(int filedescriptor);
            void set_message_received(std::string message_received);
            int  get_client_fd();
            std::string get_message_received();
            std::string get_message_put_together();
            void put_message_together();
};