/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 20:06:12 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/23 14:39:39 by nsloniow         ###   ########.fr       */
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
        std::string message;
        // std::string message_received;
        // std::string messages_put_together;
  
        public:
            ~Client();
            Client();
            Client(int filedescriptor);

            void set_client_fd(int filedescriptor);
            void set_message(std::string message);
            int  get_client_fd();
};