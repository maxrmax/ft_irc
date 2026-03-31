/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 18:40:06 by nsloniow          #+#    #+#             */
/*   Updated: 2026/01/08 18:40:10 by nsloniow         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <poll.h>
#include <vector>

/**
 * poll helpers
 *
 * Small header documenting usage of POSIX `poll` within this project.
 * - Use a `std::vector<pollfd>` to manage the set of fds monitored for events.
 * - `pollfd.events` should typically include `POLLIN` to watch for readable data.
 * - After `poll` returns, inspect `pollfd.revents` to determine what happened
 *   (e.g. `POLLIN`, `POLLHUP`, `POLLERR`). Handle `POLLHUP`/`POLLERR` by
 *   closing and cleaning up the corresponding client.
 *
 * Note: ensure fds are removed from the vector before closing to avoid
 * reusing closed descriptors in subsequent iterations.
 */