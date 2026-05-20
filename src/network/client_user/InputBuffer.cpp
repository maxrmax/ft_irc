/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputBuffer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:05:36 by student           #+#    #+#             */
/*   Updated: 2026/01/30 12:05:36 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "InputBuffer.hpp"
#include "inputBuffer.hpp"
#include <stdexcept>

InputBuffer::InputBuffer() {}

void InputBuffer::append(const std::string& data)
{
    _buffer += data;
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

    // pos == message length WITHOUT \r\n
    if (pos > IRC_MAX_LINE - 2)
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
