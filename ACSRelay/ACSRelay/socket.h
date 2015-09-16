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
 * @brief Class that provides basic communication on an UDP socket.
 *        It holds information about the other device such as
 *        its address and its UDP port, and also the local socket
 *        file descriptor. This class also provides methods to
 *        send information through the socket.
 */
class Socket {
    
public:

    // CTOR
    
    /**
     * @brief Socket object constructor
     * @param local_port Port on which to listen as a long integer.
     */
    Socket ( const unsigned int local_port );
    /**
     * @brief Socket object constructor
     * @param host Hostname of the UDP correspondent.
     * @param local_port Port on which to listen as a long integer.
     * @param remote_port Port on which to send as a long integer.
     */
    Socket ( const std::string host, const unsigned int local_port, const unsigned int remote_port );
    virtual ~Socket() {}
    
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
     * @brief Sends a message on the socket. Note that it is
     *        required to set the address beforehand, either
     *        via SetAddress() or via SetHost().
     * @param msg Byte array to send.
     * @param len Length of the byte array.
     */
    void Send ( const char* msg, const size_t len ) const;
    
private:

    // CTOR
    
    /**
     * @brief Socket object constructor.
     */
    Socket () {}
    
    // VARS
    
    int mSockFd;
    std::string mHost;
    struct sockaddr_in mCa;
    unsigned int mRemotePort;
    unsigned int mLocalPort;
};

#endif // __SOCKET_H
