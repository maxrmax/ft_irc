/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdMode.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 18:56:33 by mring             #+#    #+#             */
/*   Updated: 2026/03/09 18:56:33 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CMDMODE_HPP
#define CMDMODE_HPP
#include "command.hpp"

class CmdMode : public Command
{
public:
    void execute(Server &server,
                 ClientUser &client,
                 const ParsedCommand &cmd);
};

#endif // CMDMODE_HPP