#ifndef __PLUGIN_H
#define __PLUGIN_H

#include <chrono>
#include <string>

#include <socket.h>

/**
 * @brief Class that handles the communication logic with an UDP
 *        plugin. It keeps track of what messages the plugin is
 *        waiting for and provides a socket on which to communicate
 *        with the UDP plugin.
 */
class PluginHandler {
    
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<Clock> Time;
    typedef std::chrono::milliseconds Ms;
    
public:
    
    /**
     * @brief PluginHandler object constructor
     * @param name Name of the plugin. Currently not used.
     * @param host UDP plugin address as a string.
     * @param port UDP plugin port as a long integer.
     */
    PluginHandler ( const std::string name, const std::string host, const unsigned int local_port, const unsigned int remote_port );
    /**
     * @brief PluginHandler object constructor
     * @param name Name of the plugin. Currently not used.
     * @param socket Pointer to a Socket object that will be used
     *        for communication.
     */
    PluginHandler ( const std::string name, Socket* socket );
    /**
     * @brief Implicit PluginHandler object constructor
     */
    PluginHandler () { mSocket = NULL; mCarUpdateInterval = 0; }
    virtual ~PluginHandler()
    {
        if ( mSocket != NULL )
            delete mSocket;
    }
    
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
    void SetCarUpdateInterval ( const unsigned long ri ) { mCarUpdateInterval = ri; }
    
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
     * @param cid Session ID as represented in Assetto Corsa.
     */
    void RequestSessionInfo ( const short sid ) { mRequestedSessionInfo[ sid ] = true; }
    /**
     * @brief Notifies the arrival of an ACSP_SESSION_INFO packet.
     * @param cid Session ID as represented in Assetto Corsa.
     */
    void SessionInfoArrived ( const short sid ) { mRequestedSessionInfo[ sid ] = false; }
    
    /**
     * @brief Checks if the plugin is waiting for an ACSP_CAR_UPDATE packet.
     * @param cid Car ID as represented in Assetto Corsa.
     * @return Boolean telling if the plugin expects a new update.
     */
    bool IsWaitingCarUpdate ( const short cid );
    /**
     * @brief Notifies the arrival of an ACSP_CAR_UPDATE packet.
     * @param cid Car ID as represented in Assetto Corsa.
     */
    void CarUpdateArrived ( const short cid );
private:
    // VARS
    
    std::string mName;
    
    Socket* mSocket;
    unsigned long mCarUpdateInterval;
    
    bool mRequestedCarInfo[ 64 ];
    bool mRequestedSessionInfo[ 64 ];
    
    Time mLastUpdate[ 64 ];
};

#endif // __PLUGIN_H
