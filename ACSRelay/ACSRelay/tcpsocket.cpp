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

#include "log.h"
#include "tcpsocket.h"

#ifdef _WIN32
    #include <ws2tcpip.h>
#endif

#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

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
    retval = accept(mSockFd, reinterpret_cast<struct sockaddr*> ( &mCa ), &len );

    mRemotePort = ntohs ( mCa.sin_port );

    return retval;
}

void TCPSocket::SetBlocking( const bool blocking )
{
#ifdef _WIN32
    unsigned long block;
    block = blocking ? 1 : 0;
    ioctlsocket ( mSockFd, FIONBIO, &block );
#else
    int opts;

    if ( blocking )
    {
        opts = fcntl (mSockFd, F_GETFL );
        opts = opts & ( ~O_NONBLOCK );
        fcntl ( mSockFd, F_SETFL, opts );
    }
    else
        fcntl ( mSockFd, F_SETFL, O_NONBLOCK );
#endif
}

int TCPSocket::Connect( unsigned short timeout )
{
    fd_set rd, wr;
    long status;
    struct timeval tv;
    char err;
    socklen_t len = sizeof ( int );

    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    FD_ZERO ( &rd );
    FD_SET ( mSockFd, &rd );
    FD_ZERO ( &wr );
    FD_SET ( mSockFd, &wr );

    SetBlocking ( false );

    if ( ( status = connect ( mSockFd, reinterpret_cast<struct sockaddr*> ( &mCa ), sizeof ( mCa ) ) ) == -1 )
    {
        if ( errno != EINPROGRESS )
        {
            return static_cast<int> ( status );
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

    if ( err == 0 )
    {
        // TCP connection established.
        // Make the socket blocking again...

        SetBlocking ( true );

        // And return from the function.

        return 0;
    }

    return -1;
}

void TCPSocket::Close ()
{
    char msg[ 32 ];

#ifdef _WIN32
    shutdown ( mSockFd, SD_BOTH );
#else
    shutdown ( mSockFd, SHUT_RDWR );
#endif

    while ( recv ( mSockFd, msg, 32, 0 ) > 0 );

    close ( mSockFd );
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

    if ( bind ( mSockFd, reinterpret_cast<struct sockaddr*> ( &sa ), sizeof ( sa ) ) < 0 )
    {
        Log::e() << "Failed to bind TCP socket for host " << mHost << ":" << mLocalPort;
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

    if ( type == FROM_FD )
    {
        mSockFd = param;

        socklen_t addrlen = sizeof ( mCa );
        if ( getpeername ( mSockFd, reinterpret_cast<struct sockaddr*> ( &mCa ), &addrlen ) < 0 )
        {
            mIsConnected = false;
        }
        else
        {
            mIsConnected = true;

            mHost = inet_ntoa ( mCa.sin_addr );
            mRemotePort = ntohs ( mCa.sin_port );

            addrlen = sizeof ( sa );

            getsockname ( mSockFd, reinterpret_cast<struct sockaddr*> ( &sa ), &addrlen );

            mLocalPort = ntohs ( sa.sin_port );
        }
    }
    else if ( type == SERVER )
    {
        mIsConnected = false;

        mLocalPort = param;
        mHost = "127.0.0.1";

        memset ( &sa, 0, sizeof ( mCa ) );
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl ( INADDR_ANY );
        sa.sin_port = htons( mLocalPort );

        mSockFd = socket ( AF_INET, SOCK_STREAM, 0 );

        if ( bind ( mSockFd, reinterpret_cast<struct sockaddr*> ( &sa ), sizeof ( sa ) ) < 0 )
        {
            Log::e() << "Failed to bind TCP socket for host " << mHost << ":" << mLocalPort;
        }

        if ( listen ( mSockFd, 5 ) < 0 )
        {
            Log::e() << "Failed to listen on TCP port " << mLocalPort;
        }

        memset ( &mCa, 0, sizeof ( mCa ) );
        mCa.sin_addr.s_addr = INADDR_NONE;
    }
}

TCPSocket::~TCPSocket()
{
    Close ();
}
