/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_packet.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:03:11 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/03 12:50:24 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc_packet.hpp"

irc_packet::~irc_packet(){};

irc_packet::irc_packet():
    user('\0'), command('\0'), command_int(0), message('\0'){};
        
void    irc_packet::set(char t_user, char t_command, char t_message)
{
    //save tokens from parsing as per irc protokol
    //do not parce each char but search for string ?\r\n ?
    
};