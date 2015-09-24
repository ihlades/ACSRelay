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

#ifdef DEBUG
    #include <fstream>
#endif

#define BUFFER_SIZE 512

/**
 * @brief Main class whose instance handles the messages exchange between
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
     * It creates the ACSRelay object if none exists.
     */
    static ACSRelay* Build ();
    virtual ~ACSRelay ();
    
    // METHODS
    
    /**
     * @brief Reads configuration from the specified INI file.
     * @param ini_fn Name of file containing configuration options in INI format.
     */
    void ReadConfiguration ( std::string ini_fn );
    void SetRemotePort ( const unsigned int port ) { mRemotePort = port; }
    void SetLocalPort ( const unsigned int port ) { mLocalPort = port; }
    void SetRelayPort ( const unsigned int port ) { mRelayPort = port; }
    /**
     * @brief Adds a plugin in the specified list.
     * @param plugin Pointer to a PluginHandler object associated with a plugin.
     */
    void AddPeer ( PeerConnection *plugin );
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
    
    // ENUMS
    
    enum ServerType
    {
        RELAY,
        AC
    };
    
    // VARS
    
    static ACSRelay* mInstance;
    
    ServerType mServerType;
    
    std::string mHost;
    unsigned int mLocalPort;
    unsigned int mRemotePort;
    unsigned int mRelayPort;
    int mMaxFd;
    Socket* mServerSocket;
    TCPSocket* mRelaySocket;
    
    std::map< int, PeerConnection* > mPeers;
    
    unsigned long mMri;
    
    const static unsigned short mTCPTimeout = 5;
    
#ifdef DEBUG
    std::ofstream dOut;
#endif
};
