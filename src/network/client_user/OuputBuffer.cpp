/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OuputBuffer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student>                          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:11:58 by student           #+#    #+#             */
/*   Updated: 2026/02/05 13:11:58 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "OutputBuffer.hpp"

OutputBuffer::~OutputBuffer() {}
OutputBuffer::OutputBuffer() {}

void OutputBuffer::append(const std::string& data)
{
    buffer += data;
    buffer += "\r\n";
}

std::string &OutputBuffer::get_buffer()
{
    return buffer;
}
