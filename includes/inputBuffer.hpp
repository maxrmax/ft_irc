/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inputBuffer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:07:12 by ngoyat            #+#    #+#             */
/*   Updated: 2026/01/30 12:07:12 by ngoyat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUTBUFFER_HPP
#define INPUTBUFFER_HPP

#include <string>

class InputBuffer
{
public:
    InputBuffer();

    void                append(const std::string& data);
    bool                hasLine() const;
    std::string         popLine();
    const std::string   &get_buffer() const;

private:
    std::string _buffer;

    static const size_t IRC_MAX_LINE = 510; // excluding \r\n
};

#endif // INPUTBUFFER_HPP
