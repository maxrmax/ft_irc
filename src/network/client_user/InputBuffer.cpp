/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputBuffer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:05:36 by ngoyat            #+#    #+#             */
/*   Updated: 2026/01/30 12:05:36 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "InputBuffer.hpp"
#include "inputBuffer.hpp"
#include <stdexcept>

InputBuffer::InputBuffer() {}

void InputBuffer::append(const std::string& data)
{
    _buffer += data;

    /*
     * IRC rule:
     * If the client sends more than 512 bytes total
     * without a CRLF, the connection must be closed.
     */
    // TODO 1
    if (_buffer.find("\r\n") == std::string::npos &&
        _buffer.size() > IRC_MAX_LINE + 2)
    {
        _buffer.clear();
        throw std::length_error("ERROR :Line too long");
    }
}

bool InputBuffer::hasLine() const
{
    return _buffer.find("\r\n") != std::string::npos;
}

std::string InputBuffer::popLine()
{
    size_t pos = _buffer.find("\r\n");
    if (pos == std::string::npos)
        return "";

    /*
     * pos == message length WITHOUT \r\n
     */
    if (pos > IRC_MAX_LINE)
    {
        // Discard invalid line
        _buffer.erase(0, pos + 2);
        return "ERROR :Line too long";
    }

    std::string line = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 2);
    return line;
}

const std::string &InputBuffer::get_buffer() const
{
    return _buffer;
}

//TODO integration with recv()/poll() loop