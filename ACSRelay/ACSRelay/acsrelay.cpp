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
      mRequestedInterval(0),
      mSetInterval(0)
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

    if ( Log::GetOutputLevel() >= Log::VERBOSE_LVL )
    {
        // Check if the PeerConnection has an UDP socket. If so,
        // it is a simple plugin.
        if (  dynamic_cast<UDPSocket*>(plugin -> GetSocket()) != NULL )
        {
            Log::v() << "Adding new plugin " << plugin -> Name() <<  " (" << host << ":" << plugin -> GetSocket () -> RemotePort () << "). " << "Listening on local UDP port " << plugin -> GetSocket() -> LocalPort() << ".";
        }
        else
        {
            // If it has a TCP socket (the only other option), it is a downstream
            // instance of ACSRelay.
            Log::v() << "Adding new downstream relay " << plugin -> Name() <<  " (" << host << ":" << plugin -> GetSocket () -> RemotePort () << "). " << "Listening on local TCP port " << plugin -> GetSocket() -> LocalPort() << ".";
        }
    }

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

    if ( n < 1 )
    {
        // We'll get here only if the peer's TCP socket has been disconnected.
        // Destroy the PeerConnection to make sure we close the socket on our side.
        for ( auto p = mPeers.begin (); p != mPeers.end (); ++p )
        {
            if ( p -> second == plugin )
            {
                Log::v () << "TCP read error from " << plugin -> Name() << ". Closing connection and removing downstream relay.";
                delete p -> second;
                mPeers.erase( p -> first );
                break;
            }
        }
        return;
    }

    Log::d() << "Caught message from " << plugin -> Name () << "!" << Log::Packet ( msg, n );

    // Only relay packets that can actually be sent by a plugin.
    // Everything else must be bogus.

    switch ( static_cast<int8_t> ( msg[ 0 ] ) )
    {
        case ACSProtocol::ACSP_REALTIMEPOS_INTERVAL:
        case ACSProtocol::ACSP_GET_CAR_INFO:
        case ACSProtocol::ACSP_SEND_CHAT:
        case ACSProtocol::ACSP_BROADCAST_CHAT:
        case ACSProtocol::ACSP_GET_SESSION_INFO:
        case ACSProtocol::ACSP_SET_SESSION_INFO:
        case ACSProtocol::ACSP_KICK_USER:
            break;
        default:
            Log::v () << "Received an invalid packet from plugin " << plugin -> Name() << ". Dropping.";
            return;
    }


    // Only send ACSP_REALTIMEPOS_INTERVAL to server if it's lower
    // than before.
    if ( static_cast<int8_t> ( msg[ 0 ] ) == ACSProtocol::ACSP_REALTIMEPOS_INTERVAL )
    {
        if ( n >= 2 )
        {
            ri = msg[ 1 ];
            plugin -> SetCarUpdateInterval ( ri );

            // If mSetInterval is not zero, then we know the server is online
            // and we can send the packet. Set mRequestedInterval as well, so
            // that we'll resend this packet if the server doesn't start
            // giving us updates.
            if ( mSetInterval != 0 )
            {
                if ( ri < mSetInterval )
                {
                    mSetInterval = mRequestedInterval = ri;
                    Log::d () << "Relaying packet to server.";
                    mServerSocket -> Send ( msg, n );
                }
            }
            else
            {
                // The server hasn't yet responded to our update subscription.
                // Store the requested interval to send the update request packet
                // next time the server sends us a message to make sure it is
                // online.
                //
                // Do this in two cases: either this is the first
                // ACSP_REALTIMEPOS_INTERVAL we've processed, or the requested
                // interval is shorter than what we previously had.
                if ( mRequestedInterval == 0 || ri < mRequestedInterval )
                {
                    Log::d () << "Set mRequestedInterval to " << ri << " ms.";
                    mRequestedInterval = ri;
                    Log::v () << "Car update interval set to " << ri << " ms.";
                }
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
    {
        // We can get here either by encountering an error when reading from the socket
        // or if we're using a RELAY server.

        if ( mServerType == RELAY )
        {
            // Be that true, it means it has closed the TCP socket, so we
            // have to do the same, which means this ACSRelay instance has no
            // purpose.
            Log::e () << "Couldn't read from TCP socket. Has the upstream ACSRelay closed?";
            Log::e () << "Exiting program...";
            exit ( 0 );
        }
        else
        {
            // If upstream resides a classic AC game server, it means we've had
            // an error while reading from the socket. We should just ignore it.
            return;
        }
    }

    Log::d() << "Caught message from  server!" << Log::Packet ( msg, n );

    // Only relay packets that can actually be sent by the server.
    // Everything else must be bogus.

    switch ( static_cast<int8_t> ( msg[ 0 ] ) )
    {
        case ACSProtocol::ACSP_NEW_SESSION:
        case ACSProtocol::ACSP_NEW_CONNECTION:
        case ACSProtocol::ACSP_CONNECTION_CLOSED:
        case ACSProtocol::ACSP_CAR_UPDATE:
        case ACSProtocol::ACSP_CAR_INFO:
        case ACSProtocol::ACSP_END_SESSION:
        case ACSProtocol::ACSP_LAP_COMPLETED:
        case ACSProtocol::ACSP_VERSION:
        case ACSProtocol::ACSP_CHAT:
        case ACSProtocol::ACSP_CLIENT_LOADED:
        case ACSProtocol::ACSP_SESSION_INFO:
        case ACSProtocol::ACSP_ERROR:
        case ACSProtocol::ACSP_CLIENT_EVENT:
            break;
        default:
            Log::v () << "Received an invalid packet from the server. Dropping it.";
            return;
    }

    // Send realtime position update to subscribed plugins
    if ( static_cast<int8_t> ( msg[ 0 ] ) == ACSProtocol::ACSP_CAR_UPDATE )
    {
        // We've received an ACSP_CAR_UPDATE packet, so our
        // ACSP_REALTIMEPOS_INTERVAL got to the server. Take note of that.
        //
        // We're assuming that the latest update request we sent to the server
        // got there.
        if ( mRequestedInterval != 0 )
        {
            mSetInterval = mRequestedInterval;
            mRequestedInterval = 0;
        }

        for ( auto p = mPeers.begin (); p != mPeers.end (); ++p )
        {
            // Send ACSP_CAR_UPDATE packets to any plugin that is interested.
            // A plugin will expect an ACSP_CAR_UPDATE packet only after
            // its car update interval has elapsed.
            //
            // Also, if the plugin has the minimum car update interval make
            // sure to send it the packet.
            Time t = Clock::now ();

            if ( p -> second -> CarUpdateInterval () == mSetInterval ||
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

    // The server sent us a message so it's online. This is the right time
    // to check if we have to send it a ACSP_REALTIMEPOS_INTERVAL packet.
    if ( mRequestedInterval != 0 )
    {
        msg[0] = ACSProtocol::ACSP_REALTIMEPOS_INTERVAL;
        *(reinterpret_cast<uint16_t*>( msg + 1 )) = mRequestedInterval;

        // Send the ACSP_REALTIMEPOS_INTERVAL packet to the server:
        mServerSocket -> Send ( msg, 3 );

        Log::d () << "Sent packet to server:" << Log::Packet ( msg, 3 );
    }
}

__attribute__((__noreturn__)) void ACSRelay::Start()
{
    fd_set fds;

    PeerConnection* plugin;
    TCPSocket* tcp_socket;
    std::string incoming_relay_name;

    if ( mLocalPort == 0 || mRemotePort == 0 )
    {
        Log::e () << "Invalid port configuration. Check the settings file is written properly.";
        Log::e () << "Exiting program...";
        exit ( 2 );
    }

    Log::i () << "Relay starting...";

    switch ( mServerType )
    {
        case AC:
            mServerSocket = new UDPSocket ( mLocalPort );
            Log::v () << "Listening on local UDP port " << mLocalPort << " for messages from the server...";
            break;
        case RELAY:
            tcp_socket = new TCPSocket ( mHost, mRemotePort );
            Log::v () << "Trying to connect with another relay (" << mHost << ":" << mRemotePort << ") via TCP" << "...";

            if ( tcp_socket -> Connect ( kTCPTimeout ) >= 0 )
            {
                Log::v () << "Connected!";
            }
            else
            {
                Log::v () << "Failed! ACSRelay is closing.";
                exit ( 1 );
            }
            mServerSocket = reinterpret_cast<Socket*> ( tcp_socket );

            break;
    }

    if ( mRelayPort != 0 )
    {
        mRelaySocket = new TCPSocket ( TCPSocket::SERVER, mRelayPort );
        Log::v () << "Configured as a ACSRelay server. Listening for messages from other relays on local TCP port " << mRelayPort << ".";
    }

    Log::i () << "Relay started!";


    if ( mServerSocket -> Fd () > mMaxFd )
        mMaxFd = mServerSocket -> Fd ();

    if ( mRelaySocket != NULL )
        mMaxFd = ( mMaxFd < mRelaySocket -> Fd () ) ? mRelaySocket -> Fd () : mMaxFd;

    // Initially disable realtime car updates for all plugins.
    for ( auto p = mPeers.begin(); p != mPeers.end (); ++p )
    {
        p -> second -> SetCarUpdateInterval ( 0 );
    }

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
                    // Message came from the server. Treat it as such.
                    RelayFromServer ();
                }
                else
                {
                    // We either received a message from a plugin,
                    // or a connection request from a downstream
                    // ACSRelay instance.

                    if ( mRelaySocket != NULL && i == mRelaySocket -> Fd () )
                    {
                        // Allow TCP connection and add it to our downstream peer list.

                        tcp_socket = new TCPSocket ( TCPSocket::FROM_FD, mRelaySocket -> Accept() );

                        // Generate a unique identifier for the newly accepted ACSRelay connection
                        // based on the current size of the peer list.
                        //
                        // Note: This isn't required to be unique. However, it's preferrable for
                        // logging purposes.
                        incoming_relay_name = "RELAY_";
                        incoming_relay_name += int( mPeers.size() );

                        plugin = new PeerConnection ( incoming_relay_name, reinterpret_cast<Socket*> ( tcp_socket ) );
                        AddPeer ( plugin );
                    }
                    else
                    {
                        // Message came from a plugin. Treat it as such.
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
