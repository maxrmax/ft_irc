/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_packet.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 12:01:42 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/03 12:03:36 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class irc_packet
{
    private:
        char    user;
        char    command;
        int     command_int;
        char    message;

    public:
        ~irc_packet();
        irc_packet();
        void set(char t_user, char t_command, char t_message);
};