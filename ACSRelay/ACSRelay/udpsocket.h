#ifndef __UDPSOCKET_H
#define __UDPSOCKET_H

#include "socket.h"

#define UDP_BUFFER_SIZE 1024

/**
 * @brief Subclass of the Socket virtual class.
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
