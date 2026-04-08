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

#pragma once

#include <string>

/**
 * InputBuffer
 *
 * Per-client owned buffer for raw data read from a socket.
 *
 * - `append(data)`: append raw bytes/read from the socket to the internal buffer.
 * - `hasLine()`: true if at least one complete IRC line is present (CRLF or LF).
 * - `popLine()`: pop and return the next complete line (trailing CRLF/LF removed).
 * - `get_buffer()`: read-only access to the remaining partial data.
 *
 * Semantics and expectations:
 * - Enforces IRC line semantics via `IRC_MAX_LINE` (510) which is the maximum
 *   content length excluding terminating CRLF. Callers should validate length
 *   when necessary (or rely on higher-level checks).
 * - The buffer does not parse commands; it only detects line boundaries.
 * - Not thread-safe — synchronize externally if accessed from multiple threads.
 */
class InputBuffer
{
public:
    InputBuffer();

    /**
     * Append raw data received from the socket to the internal buffer.
     * Caller is responsible for enforcing per-line length limits if desired.
     */
    void                append(const std::string& data);

    /**
     * Returns true if the buffer contains at least one complete IRC line
     * (terminated by CRLF or LF depending on parsing policy).
     */
    bool                hasLine() const;

    /**
     * Pop and return the next complete line (without trailing CRLF).
     * If no complete line is available behavior is the caller's responsibility
     * (current implementation returns an empty string or may assert).
     */
    std::string         popLine();

    // Access to raw buffer (read-only reference)
    const std::string   &get_buffer() const;

private:
    std::string _buffer;

    // IRC standard max message length is 512 including CRLF; store 510 here
    static const size_t IRC_MAX_LINE = 512;
};

