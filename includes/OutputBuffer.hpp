/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OutputBuffer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:14:22 by nsloniow          #+#    #+#             */
/*   Updated: 2026/02/05 13:14:22 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once

#include <string>
// #include "ircserv.hpp"

class OutputBuffer
{
    private:
        std::string buffer;

        public:
        ~OutputBuffer();
        OutputBuffer();
        
        
        const std::string   &get_buffer() const;

        void                append(const std::string& data);
        // bool                hasLine() const;
        std::string         popLine();


};


