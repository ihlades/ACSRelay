#include "tcpsocket.h"

#include <fcntl.h>
#include <iostream>
#include <unistd.h>

long TCPSocket::Send ( const char* msg, const size_t len ) const
{
    return send ( mSockFd, msg, len, 0 );
}

long TCPSocket::Read ( char *msg, const size_t len )
{
    return recv ( mSockFd, msg, len, 0 );
}

int TCPSocket::Accept()
{
    int retval;
    
    socklen_t len = sizeof ( mCa );
    retval = accept(mSockFd, (struct sockaddr*) &mCa, &len );
    
    mRemotePort = ntohs ( mCa.sin_port );
    
    return retval;
}

int TCPSocket::Connect( unsigned short timeout )
{
    fd_set rd, wr;
    long status;
    struct timeval tv;
    int err;
    socklen_t len = sizeof ( int );
    int opts;
    
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    
    FD_ZERO ( &rd );
    FD_SET ( mSockFd, &rd );
    FD_ZERO ( &wr );
    FD_SET ( mSockFd, &wr );
    
    fcntl ( mSockFd, F_SETFL, O_NONBLOCK );
    
    
    if ( ( status = connect ( mSockFd, (struct sockaddr*) &mCa, sizeof ( mCa ) ) ) == -1 )
    {
        if ( errno != EINPROGRESS )
        {
            return (int) status;
        }
    }
    
    status = select (mSockFd + 1, &rd, &wr, NULL, &tv);
    
    if ( !FD_ISSET ( mSockFd, &rd ) && !FD_ISSET ( mSockFd, &wr ) )
    {
        return -2;
    }
    
    if ( getsockopt ( mSockFd, SOL_SOCKET, SO_ERROR, &err, &len ) < 0)
    {
        return -2;
    }
    
    if ( errno == 0 )
    {
        // TCP connection established.
        // Make the socket blocking again...
        
        opts = fcntl (mSockFd, F_GETFL );
        opts = opts & ( ~O_NONBLOCK );
        fcntl ( mSockFd, F_SETFL, opts );
        
        // And return from the function.
        
        return 0;
    }
    
    return -1;
}

TCPSocket::TCPSocket ( const std::string host, const unsigned int remote_port )
{
    struct sockaddr_in sa;
    
    mIsConnected = false;
    
    mHost = host;
    mLocalPort = 0;
    mRemotePort = remote_port;
    
    memset ( &sa, 0, sizeof ( mCa ) );
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl ( INADDR_ANY );
    sa.sin_port = htons ( mLocalPort );
    
    mSockFd = socket ( AF_INET, SOCK_STREAM, 0 );
    
    if ( bind ( mSockFd, ( struct sockaddr* )&sa, sizeof ( sa ) ) < 0 )
    {
        std::cerr << "Failed to bind TCP socket for host " << mHost << ":" << mLocalPort << std::endl;
    }
    
    // Now save host and port information in mCa
    // to be used when sending packets to the client plugin.
    
    memset ( &mCa, 0, sizeof ( mCa ) );
    mCa.sin_family = AF_INET;
    inet_pton ( AF_INET, host.c_str (), &( mCa.sin_addr ) );
    mCa.sin_port = htons ( mRemotePort );
}

TCPSocket::TCPSocket ( Type type, const unsigned int param )
{
    struct sockaddr_in sa;
    
    if ( type == Type::FROM_FD )
    {
        mSockFd = param;

        socklen_t addrlen = sizeof ( mCa );
        if ( getpeername ( mSockFd, ( struct sockaddr* ) &mCa, &addrlen ) < 0 )
        {
            mIsConnected = false;
        }
        else
        {
            mIsConnected = true;
            
            mHost = inet_ntoa ( mCa.sin_addr );
            mRemotePort = ntohs ( mCa.sin_port );
            
            addrlen = sizeof ( sa );
            
            getsockname ( mSockFd, (struct sockaddr* ) &sa, &addrlen );
            
            mLocalPort = ntohs ( sa.sin_port );
        }
    }
    else if ( type == Type::SERVER )
    {
        mIsConnected = false;
        
        mLocalPort = param;
        mHost = "127.0.0.1";
        
        memset ( &sa, 0, sizeof ( mCa ) );
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl ( INADDR_ANY );
        sa.sin_port = htons( mLocalPort );
        
        mSockFd = socket ( AF_INET, SOCK_STREAM, 0 );
        
        if ( bind ( mSockFd, ( struct sockaddr* )&sa, sizeof ( sa ) ) < 0 )
        {
            std::cerr << "Failed to bind TCP socket for host " << mHost << ":" << mLocalPort << std::endl;
        }
        
        if ( listen ( mSockFd, 5 ) < 0 )
        {
            std::cerr << "Failed to listen on TCP port " << mLocalPort << std::endl;
        }
        
        memset ( &mCa, 0, sizeof ( mCa ) );
        mCa.sin_addr.s_addr = INADDR_NONE;
    }
}