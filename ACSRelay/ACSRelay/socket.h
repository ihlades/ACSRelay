#ifndef __SOCKET_H
#define __SOCKET_H

#include <string>

#ifdef WIN32
   #include <winsock.h>
#else
   #include <sys/socket.h>
   #include <netinet/in.h>
#endif

#ifdef WIN32
    #include "inet_help.h"
#else
    #include <arpa/inet.h>
#endif

/**
 * @brief Class that provides basic abstraction over socket communcation.
 *        It holds information about the other device such as
 *        its address and its remote port, and also the local socket
 *        file descriptor. This class also provides methods to
 *        send and receive information through the socket.
 *
 *        This is a pure virtual class so it can't be instantiated.
 *        Instead, use one of its subclasses (TCPSocket and UDPSocket).
 */
class Socket {
    
public:
    
    // CTOR/DCTOR
    
    virtual ~Socket () {};
    
    // METHODS
    
    /**
     * @brief Retrieves the file descriptor of the socket.
     * @return File descriptor as an integer.
     */
    int Fd () const { return mSockFd; }
    
    /**
     * @brief Retrieves the hostname of the UDP correspondent.
     * @return Hostname as a string.
     */
    std::string Host () const { return mHost; }
    
    /**
     * @brief Sets the address of the UDP correspondent.
     * @param addr Address as an unsigned integer.
     */
    void SetAddress ( const unsigned int addr ) { mCa.sin_addr.s_addr = addr; }
    
    /**
     * @brief Sets the hostname of the UDP correspondent.
     *        This also changes the correspondent's address.
     * @param host Hostname as a string.
     */
    void SetHost ( const std::string host )
    {
        mHost = host;
        mCa.sin_family = AF_INET;
        inet_pton ( AF_INET, host.c_str (), &( mCa.sin_addr ) );
    }
    
    /**
     * @brief Retrieves the port used by the socket.
     * @return Port as a long integer.
     */
    long LocalPort () const { return mLocalPort; }
    /**
     * @brief Retrieves the port used by the socket.
     * @return Port as a long integer.
     */
    long RemotePort () const { return mRemotePort; }
    /**
     * @brief Sets the port to be used by the socket.
     * @param port Port as a long integer.
     */
    void SetRemotePort ( const unsigned int port ) { mRemotePort = port; mCa.sin_port = htons ( port ); }
    /**
     * @brief Send bytes through the socket.
     * @param msg Array containing bytes.
     * @param len Number of bytes in the array.
     * @return -1 on error, otherwise the number of sent bytes.
     */
    virtual long Send ( const char* msg, const size_t len ) const = 0;
    /**
     * @brief Read bytes from the socket (if any available).
     * @param msg Pointer to a byte array to hold the incoming data.
     * @param len Maximum number of bytes to read.
     * @return -1 on error, otherwise the number of read bytes.
     */
    virtual long Read ( char *msg, const size_t len ) = 0;
protected:
    
    // VARS
    
    int mSockFd;
    std::string mHost;
    struct sockaddr_in mCa;
    unsigned int mRemotePort;
    unsigned int mLocalPort;
};

#endif // __SOCKET_H
