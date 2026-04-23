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
