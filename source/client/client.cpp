// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   client.cpp                                         :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2026/01/03 18:08:54 by nsloniow          #+#    #+#             */
// /*   Updated: 2026/02/17 12:12:38 by nsloniow         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "../../includes/client.hpp"


// client::~client()
// {
//     //close fd for client
//     if (client_fd != -1)
//     {
//         if (close(client_fd) == 0)
//         {    
//             std::cout << "Client filedescriptor closed for a good nights sleep." << std::endl;
//             client_fd = -1;
//         }
//         else 
//         {
//             std::cout << "Client filedescriptor could not be closed." << std::endl;
//         }
//     }       
        
//     //errno
// };

// client::client():client_fd(-1), passAccepted(false), registered(false){};

// client::client(int filedescriptor):client_fd(filedescriptor), passAccepted(false),registered(false){};


// void client::set_client_fd(int filedescriptor)
// {
//     client_fd = filedescriptor;
// };

// int client::get_client_fd()
// {
//     return client_fd;
// };

// InputBuffer &client::get_inputBuffer()
// {
//     return inputBuffer;
// };

// // Identity

// void client::setNickname(const std::string& nick)
// {
//     nickname = nick;
// }

// void client::setUsername(const std::string& user)
// {
//     username = user;
// }

// void client::getNickname(std::string& nick) const
// {
//     nick = nickname;
// }

// void client::getUsername(std::string& user) const
// {
//     user = username;
// }

// // Auth state

// void client::setPassAccepted(bool accepted)
// {
//     passAccepted = accepted;
// }

// bool client::isPassAccepted() const
// {
//     return passAccepted;
// }

// void client::setRegistered(bool reg)
// {
//     registered = reg;
// }

// bool client::isRegistered() const
// {
//     return registered;
// }

// bool client::isReadyToRegister() const
// {
//     if (passAccepted == false)
//         return false;
//     if (nickname.empty() || username.empty())
//         return false;
//     return true;
// }

