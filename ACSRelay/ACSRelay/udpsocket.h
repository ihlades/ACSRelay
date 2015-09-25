/*
 Copyright 2015 Victor Nicolae.

 This file is part of ACSRelay.

 ACSRelay is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ACSRelay is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with ACSRelay.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __UDPSOCKET_H
#define __UDPSOCKET_H

#include "socket.h"

#define UDP_BUFFER_SIZE 1024

/**
 * @class UDPSocket
 * @brief Provides communication over an UDP socket.
 *        Subclass of the Socket virtual class.
 *        UDPSocket provides basic functionality for communication
 *        over a UDP socket.
 */
class UDPSocket : public Socket
{
    
public:
    
    // CTOR
    
    /**
     * @brief UDPSocket object constructor
     * @param local_port Port on which to listen as a long integer.
     */
    UDPSocket ( const unsigned int local_port );
    /**
     * @brief UDPSocket object constructor
     * @param host Hostname of the UDP correspondent.
     * @param local_port Port on which to listen as a long integer.
     * @param remote_port Port on which to send as a long integer.
     */
    UDPSocket ( const std::string host, const unsigned int local_port, const unsigned int remote_port );
    
    virtual ~UDPSocket() {}
    /**
     * @brief Send bytes through the socket.
     * @param msg Array containing bytes.
     * @param len Number of bytes in the array.
     * @return -1 on error, otherwise the number of sent bytes.
     */
    long Send ( const char* msg, const size_t len ) const;
    /**
     * @brief Read bytes from the socket (if any available).
     * @param msg Pointer to a byte array to hold the incoming data.
     * @param len Maximum number of bytes to read.
     * @return -1 on error, otherwise the number of read bytes.
     */
    long Read ( char *msg, const size_t len );
    
private:
    
    // CTOR
    
    /**
     * @brief UDPSocket object constructor.
     */
    UDPSocket () {}
};

#endif
