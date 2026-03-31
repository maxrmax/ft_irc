/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isSpecial.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 22:52:02 by nsloniow          #+#    #+#             */
/*   Updated: 2026/03/09 20:21:25 by mring            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker.hpp"

bool isspecial_MACdarwin(int i)
{
    // '_' not in RFC 1459 but required for irssi
    if (i == '-' || i == '[' || i == ']' || i == '`' || i == '^' || 
        i == '{' || i == '}' || i == '\\' || i == '_')
        return true;
    return false;
}
