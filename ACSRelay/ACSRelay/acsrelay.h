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

#include <string>

#include "INIReader.h"
#include "peerconnection.h"
#include "ACSProtocol.h"
#include "socket.h"
#include "tcpsocket.h"
#include "configuration.h"

#include <queue>

#ifdef DEBUG
    #include <fstream>
#endif

#define BUFFER_SIZE 512

/**
 * @class ACSRelay
 * @brief Main class for handling ACSP messages.
 *        Main class whose instance handles the messages exchange between
 *        the Assetto Corsa Server and the various UDP plugins. It establishes
 *        the connection to the server and initiates communication with the
 *        plugins defined in the configuration file. It then relays messages
 *        from the server to the UDP plugins and the other way around.
 */
class ACSRelay
{
public:
    // CTOR

    /**
     * @brief Returns pointer to the ACSRelay instance.
     * If no ACSRelay object exists, it constructs one and
     * returns a pointer to it.
     */
    static ACSRelay* Build ( Configuration::RelayParams params );
    virtual ~ACSRelay ();
    
    // METHODS
    
    /**
     * @brief Reads configuration from the specified INI file.
     * @param ini_fn Name of file containing configuration options in INI format.
     */
    void ReadConfiguration ( std::string ini_fn );
    /**
     * @brief Set the remote server port.
     *        This will be either the UDP port on which the AC game server
     *        will listen on, or the TCP port on which another ACSRelay software
     *        will listen on.
     * @param port Port number as an unsigned integer.
     */
    void SetRemotePort ( const unsigned int port ) { mRemotePort = port; }
    /**
     * @brief Set the local server port.
     *        This will be the UDP port on which ACSRelay will listen for
     *        packets from the AC server.
     * @param port Port number as an unsigned integer.
     */
    void SetLocalPort ( const unsigned int port ) { mLocalPort = port; }
    /**
     * @brief Set the local relay port.
     *        This will be the TCP port on which ACSRelay will listen for
     *        packets from another ACSRelay instance.
     * @param port Port number as an unsigned integer.
     */
    void SetRelayPort ( const unsigned int port ) { mRelayPort = port; }
    /**
     * @brief Adds a plugin in the specified list.
     * @param plugin Pointer to a PluginHandler object associated with a plugin.
     */
    void AddPeer ( PeerConnection *plugin );

    /**
     * @brief Constructs and adds plugins based on a list of PluginParams.
     * @param plugins std::list of Configuration::PluginParams.
     */
    void AddPeer ( Configuration::PluginParams plugin );
    /**
     * @brief Monitors traffic between AC Server and UDP plugins.
     */
    void Start ();
    
private:
    
    // CTOR

    /**
     * @brief ACSRelay object constructor.
     */
    ACSRelay ();
    ACSRelay ( Configuration::RelayParams params );
    
    // METHODS
    
    /**
     * @brief Reads, interprets and relays datagrams coming from an UDP plugin.
     * @param plugin Pointer to a Plugin object, associated with the UDP plugin that generated the datagram.
     */
    void RelayFromPlugin ( PeerConnection* plugin );
    /**
     * @brief Reads, interprets and relays datagrams coming from the AC Server.
     */
    void RelayFromServer ();
    
    void OutputPacket ( const char* msg, long n );
    
    // VARS
    
    static ACSRelay* mInstance;
    
    Configuration::ServerType mServerType;
    
    std::string mHost;
    unsigned int mLocalPort;
    unsigned int mRemotePort;
    unsigned int mRelayPort;
    int mMaxFd;
    Socket* mServerSocket;
    TCPSocket* mRelaySocket;
    
    std::map< int, PeerConnection* > mPeers;

    uint16_t mRequestedInterval;
    uint16_t mSetInterval;
    
    const static unsigned int kTCPTimeout = 30;
    
#ifdef DEBUG
    std::ofstream dOut;
#endif
};
