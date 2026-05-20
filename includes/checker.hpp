/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maxrmax <maxrmax>                           +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 12:00:00 by maxrmax           #+#    #+#             */
/*   Updated: 2026/03/19 12:00:00 by maxrmax          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

// Character / token helper utilities used across the project.
// Keep these prototypes in a small header to avoid including the
// large umbrella `ircserv.hpp` in many translation units.

bool    is_digits_only(const std::string &string);
bool    isspecial_MACdarwin(int i);
