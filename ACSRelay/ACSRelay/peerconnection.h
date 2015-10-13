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

#ifndef _peerconnection_h
#define _peerconnection_h

#include <chrono>
#include <string>

#include <socket.h>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<Clock> Time;
typedef std::chrono::milliseconds Ms;

/**
 * @class PeerConnection
 * @brief Handles communication logic with plugins.
 *        Class that handles the communication logic with an UDP
 *        plugin. It keeps track of what messages the plugin is
 *        waiting for and provides a socket on which to communicate
 *        with the UDP plugin.
 */
class PeerConnection {
    
public:
    
    /**
     * @brief PluginHandler object constructor
     * @param name Name of the plugin. Currently not used.
     * @param host UDP plugin address as a string.
     * @param local_port UDP port on which to listen as a long integer.
     * @param remote_port UDP plugin port as a long integer.
     */
    PeerConnection ( const std::string name, const std::string host, const unsigned int local_port, const unsigned int remote_port );
    /**
     * @brief PluginHandler object constructor
     * @param name Name of the plugin. Currently not used.
     * @param socket Pointer to a Socket object that will be used
     *        for communication.
     */
    PeerConnection ( const std::string name, Socket* socket );
    /**
     * @brief Implicit PluginHandler object constructor
     */
    PeerConnection () { mSocket = NULL; mCarUpdateInterval = 0; }
    virtual ~PeerConnection();
    
    /**
     * @brief Getter for the PluginHandler's identifier (name).
     * @return Name as a string.
     */
    std::string Name () const { return mName; }
    /**
     * @brief Setter for the PluginHandler's identifier (name).
     * @param name Name as a string.
     */
    void SetName ( const std::string name ) { mName = name; }
    
    /**
     * @brief Retrieves a pointer to the UDP socket used to exchange data with the plugin.
     * @return Pointer to a Socket object.
     */
    Socket* GetSocket() const { return mSocket; }
    
    /**
     * @brief Retrieves the plugin's desired interval between realtime car updates from the server.
     * @return Unsigned long representing the interval in milliseconds.
     */
    unsigned long CarUpdateInterval () const { return mCarUpdateInterval; }
    /**
     * @brief Sets the plugin's desired interval between realtime car updates from the server.
     * @param ri Unsigned long representing the interval in milliseconds.
     */
    void SetCarUpdateInterval ( const long ri ) { mCarUpdateInterval = ri; }
    
    /**
     * @brief Checks if the plugin is waiting for an ACSP_CAR_INFO packet.
     * @param cid Car ID as represented in Assetto Corsa.
     * @return Boolean telling if the plugin expects a new packet.
     */
    bool IsWaitingCarInfo ( const short cid ) const { return mRequestedCarInfo[ cid ]; }
    /**
     * @brief Requests an ACSP_CAR_INFO packet for the specified car ID.
     * @param cid Car ID as represented in Assetto Corsa.
     */
    void RequestCarInfo ( const short cid ) { mRequestedCarInfo[ cid ] = true; }
    /**
     * @brief Notifies the arrival of an ACSP_CAR_INFO packet.
     * @param cid Car ID as represented in Assetto Corsa.
     */
    void CarInfoArrived ( const short cid ) { mRequestedCarInfo[ cid ] = false; }
    
    /**
     * @brief Checks if the plugin is waiting for an ACSP_SESSION_INFO packet.
     * @param sid Session ID as represented in Assetto Corsa.
     * @return Boolean telling if the plugin expects a new packet.
     */
    bool IsWaitingSessionInfo ( const short sid ) const { return mRequestedSessionInfo[ sid ]; }
    /**
     * @brief Requests an ACSP_SESSION_INFO packet for the specified car ID.
     * @param sid Session ID as represented in Assetto Corsa.
     */
    void RequestSessionInfo ( const short sid ) { mRequestedSessionInfo[ sid ] = true; }
    /**
     * @brief Notifies the arrival of an ACSP_SESSION_INFO packet.
     * @param sid Session ID as represented in Assetto Corsa.
     */
    void SessionInfoArrived ( const short sid ) { mRequestedSessionInfo[ sid ] = false; }
    
    /**
     * @brief Checks if the plugin is waiting for an ACSP_CAR_UPDATE packet.
     * @param cid Car ID as represented in Assetto Corsa.
     * @return Boolean telling if the plugin expects a new update.
     */
    bool IsWaitingCarUpdate ( const short cid, Time time );
    /**
     * @brief Notifies the arrival of an ACSP_CAR_UPDATE packet.
     * @param cid Car ID as represented in Assetto Corsa.
     */
    void CarUpdateArrived ( const short cid, Time time );
private:
    // VARS
    
    std::string mName;
    
    Socket* mSocket;
    long mCarUpdateInterval;
    
    bool mRequestedCarInfo[ 64 ];
    bool mRequestedSessionInfo[ 64 ];
    
    Time mLastUpdate[ 64 ];

    constexpr static float kUpdateIntervalPrecision = 0.9f;
};

#endif // _peerconnection_h
