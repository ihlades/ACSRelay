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

#include "acsrelay.h"
#ifdef _WIN32
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <sys/select.h>
#endif

#include <chrono>
#include <iostream>
#include <limits.h>
#include "udpsocket.h"
#include "log.h"

ACSRelay* ACSRelay::mInstance = NULL;

ACSRelay* ACSRelay::Build ()
{
    if ( mInstance == NULL )
    {
        mInstance = new ACSRelay ();
    }

    return mInstance;
}

ACSRelay::ACSRelay ()
    : mLocalPort(-1),
      mRemotePort(-1),
      mRelayPort(-1),
      mMaxFd(0),
      mServerSocket(NULL),
      mRelaySocket(NULL),
      mMri(ULONG_MAX)
{

}

void ACSRelay::AddPeer ( PeerConnection *plugin)
{
    std::string host;

    host = plugin -> GetSocket() -> Host();

    if ( host == "")
    {
        // INCORRECT HOST AND/OR PORT FOR PLUGIN
        return;
    }

    Log::v() << "Adding plugin " << plugin -> Name() <<  " (" << host << ":" << plugin -> GetSocket () -> RemotePort () << "). " << "Listening on local UDP port " << plugin -> GetSocket() -> LocalPort() << ".";

    if ( plugin -> GetSocket () -> Fd () > mMaxFd )
        mMaxFd = plugin -> GetSocket () -> Fd ();

    mPeers[ plugin -> GetSocket() -> Fd () ] = plugin;
}

void ACSRelay::ReadConfiguration( std::string ini_fn )
{
    PeerConnection *p;
    std::vector< std::string > sections;

    INIReader *ir = new INIReader ();
    ir -> parse ( ini_fn );

    mLocalPort = static_cast<unsigned int> ( ir -> GetInteger ( "SERVER", "RELAY_PORT", 0 ) );
    mRemotePort = static_cast<unsigned int> ( ir -> GetInteger( "SERVER", "SERVER_PORT", 0 ) );
    mServerType = ir -> GetString ( "SERVER", "TYPE", "AC" ) == "RELAY" ? RELAY : AC;
    mHost = ir -> GetString ( "SERVER", "IP", "127.0.0.1" );

    sections = ir -> Sections ();

    for ( unsigned int i = 0; i < sections.size (); i += 1 )
    {
        if ( sections[ i ].substr ( 0, 7 ) == "PLUGIN_" )
        {
            // ADD NEW PLUGIN

            p = new PeerConnection (
                ir -> GetString ( sections[ i ], "NAME", sections[ i ] ),
                ir -> GetString ( sections[ i ], "IP", "127.0.0.1" ),
                static_cast<unsigned int> ( ir -> GetInteger ( sections[ i ], "RELAY_PORT", 0 ) ),
                static_cast<unsigned int> ( ir -> GetInteger ( sections[ i ], "PLUGIN_PORT", 0 ) )
            );

            AddPeer ( p );
        }
    }

    mRelayPort = static_cast<unsigned int> ( ir -> GetInteger ( "RELAY", "LISTEN_PORT", 0 ) );

    delete ir;
}

void ACSRelay::RelayFromPlugin ( PeerConnection* plugin )
{
    long n;
    char msg[ BUFFER_SIZE ];
    uint8_t ri;

    n = plugin -> GetSocket() -> Read ( msg, BUFFER_SIZE );

    Log::d() << "Caught message from " << plugin -> Name () << "!\n" << LogPacket ( msg, n );

    if ( n < 1 )
        // ERROR
        return;

    // Only send ACSP_REALTIMEPOS_INTERVAL to server if it's lower
    // than before.
    if ( static_cast<int8_t> ( msg[ 0 ] ) == ACSProtocol::ACSP_REALTIMEPOS_INTERVAL )
    {
        if ( n >= 3 )
        {
            ri = msg[ 1 ] << 8 | msg[ 2 ];
            plugin -> SetCarUpdateInterval ( ri );

            if ( ri < mMri )
            {
                mMri = ri;
                Log::d () << "Relaying packet to server.";
                mServerSocket -> Send ( msg, n );
            }
        }
    }
    // This plugin is requesting info about a car. Take notice and make sure to
    // relay the server's response to this plugin.
    else if ( static_cast<int8_t> ( msg[ 0 ] ) == ACSProtocol::ACSP_GET_CAR_INFO )
    {
        plugin -> RequestCarInfo ( static_cast<int8_t> ( msg[ 1 ] ) );
        Log::d () << "Relaying packet to server";
        mServerSocket -> Send ( msg, n );
    }
    // This plugin is requesting info about a session. Take notice and make sure to relay the server's response to this plugin.
    else if ( static_cast<int8_t> ( msg[ 0 ] ) == ACSProtocol::ACSP_GET_SESSION_INFO )
    {
        plugin -> RequestSessionInfo ( static_cast<int8_t> ( msg[ 1 ] ) );
        Log::d () << "Relaying packet to server";
        mServerSocket -> Send ( msg, n );
    }
    else
    {
        Log::d () << "Relaying packet to server";
        mServerSocket -> Send ( msg, n );
    }
}

void ACSRelay::RelayFromServer()
{
    long n;
    char msg[ BUFFER_SIZE ];

    n = mServerSocket -> Read ( msg, BUFFER_SIZE );

    if ( n < 1 )
        // ERROR
        return;

    Log::d() << "Caught message from  server!\n" << LogPacket ( msg, n );

    // Send realtime position update to subscribed plugins
    if ( static_cast<int8_t> ( msg[ 0 ] ) == ACSProtocol::ACSP_CAR_UPDATE )
    {
        for ( auto p = mPeers.begin (); p != mPeers.end (); ++p )
        {
            // Send ACSP_CAR_UPDATE packets to any plugin that is interested.
            // A plugin will expect an ACSP_CAR_UPDATE packet only after
            // its car update interval has elapsed.
            //
            // Also, if the plugin has the minimum car update interval make
            // sure to send it the packet.
            Time t = Clock::now ();

            if ( p -> second -> CarUpdateInterval () == mMri ||
                 p -> second -> IsWaitingCarUpdate ( static_cast<int8_t> ( msg[ 1 ] ), t ) )
            {
                Log::d () << "Relaying packet to " << p -> second -> Name ();
                p -> second -> GetSocket () -> Send ( msg, n );
                p -> second -> CarUpdateArrived ( static_cast<int8_t> ( msg[ 1 ] ), t );
            }
        }
    }
    // One or more of the plugins requested ACSP_CAR_INFO. Send it to interested plugin(s).
    else if ( static_cast<int8_t> ( msg[ 0 ] ) == ACSProtocol::ACSP_CAR_INFO )
    {
        for ( auto p = mPeers.begin (); p != mPeers.end (); ++p )
        {
            if ( p -> second -> IsWaitingCarInfo ( static_cast<int8_t> ( msg[ 1 ] ) ) )
            {
                Log::d () << "Relaying packet to " << p -> second -> Name ();
                p -> second -> GetSocket () -> Send ( msg, n );
                p -> second -> CarInfoArrived ( static_cast<int8_t> ( msg[ 1 ] ) );
            }
        }
    }
    // One or more of the plugins requested ACSP_SESSION_INFO. Send it to interested plugin(s).
    else if ( static_cast<int8_t> ( msg[ 0 ] ) == ACSProtocol::ACSP_SESSION_INFO )
    {
        for ( auto p = mPeers.begin (); p != mPeers.end (); ++p )
        {
            if ( p -> second -> IsWaitingSessionInfo ( static_cast<int8_t> ( msg[ 1 ] ) ) )
            {
                Log::d () << "Relaying packet to " << p -> second -> Name ();
                p -> second -> GetSocket () -> Send ( msg, n );
                p -> second -> SessionInfoArrived ( static_cast<int8_t> ( msg[ 1 ] ) );
            }
        }
    }
    // For other types of packets just relay the message to all plugins.
    else
    {
        for ( auto p = mPeers.begin (); p != mPeers.end (); ++p )
        {
            Log::d () << "Relaying packet to " << p -> second -> Name ();
            p -> second -> GetSocket () -> Send ( msg, n );
        }
    }
}

__attribute__((__noreturn__)) void ACSRelay::Start()
{
    fd_set fds;

    PeerConnection* plugin;
    TCPSocket* tcp_socket;

    if ( mLocalPort == 0 || mRemotePort == 0 )
    {
        Log::e () << "Trying to start non-configured relay. Aborting...";
        exit ( 2 );
    }

    switch ( mServerType )
    {
        case AC:
            mServerSocket = new UDPSocket ( mLocalPort );
            Log::v () << "Relay starting. Listening for messages from server on local UDP port " << mLocalPort << ".";
            break;
        case RELAY:
            tcp_socket = new TCPSocket ( mHost, mRemotePort );
            Log::v() << "Relay starting. Trying to connect with another relay (" << mHost << ":" << mRemotePort << ") via TCP" << "...";

            if ( tcp_socket -> Connect ( mTCPTimeout ) >= 0 )
            {
                Log::v() << " Connected!";
            }
            else
            {
                Log::v() << " Failed! ACSRelay is closing.";
                exit ( 1 );
            }
            mServerSocket = reinterpret_cast<Socket*> ( tcp_socket );

            break;
    }

    if ( mRelayPort != 0 )
    {
        mRelaySocket = new TCPSocket ( TCPSocket::SERVER, mRelayPort );
        Log::v() << "Listening for messages from other relays on local TCP port " << mRelayPort << ".";
    }


    if ( mServerSocket -> Fd () > mMaxFd )
        mMaxFd = mServerSocket -> Fd ();

    if ( mRelaySocket != NULL )
        mMaxFd = ( mMaxFd < mRelaySocket -> Fd () ) ? mRelaySocket -> Fd () : mMaxFd;

    // Initially disable realtime car updates for all plugins.
    for ( auto p = mPeers.begin(); p != mPeers.end (); ++p )
    {
        p -> second -> SetCarUpdateInterval ( 0 );
    }

    Log::i() << "Relay started.";

    while ( 1 )
    {
        FD_ZERO ( &fds );

        for ( auto p = mPeers.begin(); p != mPeers.end (); ++p )
        {
            FD_SET ( p -> second -> GetSocket () -> Fd (), &fds );

            // Check if a recently added plugin has a file descriptor
            // bigger than the previous maximum value:

            if ( p -> second -> GetSocket() -> Fd () > mMaxFd )
            {
                mMaxFd = p -> second -> GetSocket() -> Fd ();
            }
        }

        FD_SET ( mServerSocket -> Fd (), &fds );
        if ( mRelaySocket != NULL )
            FD_SET ( mRelaySocket -> Fd (), &fds );

        select ( mMaxFd + 1, &fds, NULL, NULL, NULL );

        for ( int i = 0; i <= mMaxFd; i++ )
        {
            if ( FD_ISSET ( i, &fds ) )
            {
                if ( i == mServerSocket -> Fd () )
                {
                    RelayFromServer ();
                }
                else
                {
                    if ( mRelaySocket != NULL && i == mRelaySocket -> Fd () )
                    {
                        tcp_socket = new TCPSocket ( TCPSocket::FROM_FD, mRelaySocket -> Accept() );
                        plugin = new PeerConnection ( "RELAY", reinterpret_cast<Socket*> ( tcp_socket ) );
                        AddPeer ( plugin );
                    }
                    else
                    {
                        RelayFromPlugin ( mPeers[ i ] );
                    }
                }
            }
        }
    }
}

ACSRelay::~ACSRelay()
{
}
