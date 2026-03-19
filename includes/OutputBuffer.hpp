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

/**
 * OutputBuffer
 *
 * Per-client outgoing buffer that stores messages to be written to the socket.
 * - `append` adds data to the buffer
 * - `popLine` retrieves the next outgoing line (ready to send)
 *
 * Ownership: the `ClientUser` owns its `OutputBuffer` instance; callers obtain
 * references via accessors and must not delete the buffer.
 *
 * Thread-safety: not thread-safe. Synchronize externally if accessed concurrently.
 */
class OutputBuffer
{
private:
    std::string buffer;

public:
    ~OutputBuffer();
    OutputBuffer();

    // Return mutable reference to underlying buffer for low-level access
    std::string &get_buffer();

    // Append data to be sent to the client
    void        append(const std::string& data);

    // Pop and return the next complete line from the buffer
    std::string popLine();
};


