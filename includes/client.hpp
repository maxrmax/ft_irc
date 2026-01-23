/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 20:06:12 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/15 17:55:50 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>

class client
{
    private:
        int         client_fd;
        std::string message_received;
  
        public:
            ~client();
            client();
            client(int filedescriptor);

            int set_client_fd(int filedescriptor);
            int get_client_fd();
};