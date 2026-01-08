/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 18:40:06 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/08 18:40:10 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <poll.h>
#include <vector>
// POSIX

// struct pollfd 
// {
//     int     fd;       // the socket descriptor
//     short   events;   // bitmap of events we're interested in, here POLLIN for data received and waiting
//     short   revents;  // on return, bitmap of events that occurred
// };

//check revent, what actually happened, like POLLHUP poll hang up when server closed
//invalid fd


//close fds at end