/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OuputBuffer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:11:58 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/05 13:11:58 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "../../../includes/OutputBuffer.hpp"
#include "../../../includes/ircserv.hpp" 
// #include <stdexcept>

OutputBuffer::~OutputBuffer() {}
OutputBuffer::OutputBuffer() {}

void OutputBuffer::append(const std::string& data)
{
    buffer += data;
    buffer +="\r\n";
}

// bool OutputBuffer::hasLine() const
// {
//     return _buffer.find("\r\n") != std::string::npos;
// }

// std::string OutputBuffer::popLine()
// {
//     size_t pos = buffer.find("\r\n");
//     if (pos == std::string::npos)
//         return "";


//     if (pos > IRC_MAX_LINE)
//     {
//         // Discard invalid line
//         buffer.erase(0, pos + 2);
//         return "ERROR :Line too long";
//     }

//     std::string line = buffer.substr(0, pos);
//     buffer.erase(0, pos + 2);
//     return line;
// }

std::string &OutputBuffer::get_buffer()
{
    return buffer;
}

//TODO integration with recv()/poll() loop