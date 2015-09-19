#include "log.h"
#include "udpsocket.h"

#include <iostream>

long  UDPSocket::Send ( const char* msg, const size_t len ) const
{
    return sendto ( mSockFd, msg, len, 0, ( struct sockaddr* ) &mCa, sizeof ( mCa ) );
}

long UDPSocket::Read ( char *msg, const size_t len )
{
    socklen_t l = sizeof ( mCa );
    long n;
    
    n = recvfrom( mSockFd, msg, len, 0, (struct sockaddr*) &mCa, &l );
    
    if ( n >= 1 )
    {
        mRemotePort = ntohs ( mCa.sin_port );
    }
    
    return n;
}

UDPSocket::UDPSocket ( const std::string host, const unsigned int local_port, const unsigned int remote_port )
{
    struct sockaddr_in sa;
    
    mHost = host;
    mLocalPort = local_port;
    mRemotePort = remote_port;
    
    memset ( &sa, 0, sizeof ( mCa ) );
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl ( INADDR_ANY );
    sa.sin_port = htons ( mLocalPort );
    
    mSockFd = socket ( AF_INET, SOCK_DGRAM, 0 );
    
    if ( bind ( mSockFd, ( struct sockaddr* )&sa, sizeof ( sa ) ) < 0 )
    {
        Log::e() << "Failed to bind UDP socket for host " << mHost << ":" << mLocalPort;
    }
    
    // Now save host and port information in mCa
    // to be used when sending packets to the client plugin.
    
    memset ( &mCa, 0, sizeof ( mCa ) );
    mCa.sin_family = AF_INET;
    inet_pton ( AF_INET, host.c_str (), &( mCa.sin_addr ) );
    mCa.sin_port = htons ( mRemotePort );
}

UDPSocket::UDPSocket ( const unsigned int local_port )
{
    struct sockaddr_in sa;
    mLocalPort = local_port;
    mHost = "127.0.0.1";
    
    memset ( &sa, 0, sizeof ( mCa ) );
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl ( INADDR_ANY );
    sa.sin_port = htons( mLocalPort );
    
    mSockFd = socket ( AF_INET, SOCK_DGRAM, 0 );
    
    if ( bind ( mSockFd, ( struct sockaddr* )&sa, sizeof ( sa ) ) < 0 )
    {
        Log::e() << "Failed to bind UDP socket for host " << mHost << ":" << mLocalPort;
    }
    
    memset ( &mCa, 0, sizeof ( mCa ) );
    mCa.sin_addr.s_addr = INADDR_NONE;
}