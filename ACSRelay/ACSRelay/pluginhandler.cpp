#include "pluginhandler.h"

#include "udpsocket.h"

PluginHandler::PluginHandler ( const std::string name, const std::string host, const unsigned int local_port, const unsigned int remote_port )
{
    mName = name;
    mSocket = new UDPSocket ( host, local_port, remote_port );
    
    mCarUpdateInterval = 0;
    
    for ( unsigned short i = 0; i < 64; i += 1 )
    {
        mRequestedCarInfo[ i ] = mRequestedSessionInfo[ i ] = false;
    }
}

PluginHandler::PluginHandler ( const std::string name, Socket* socket )
{
    mName = name;
    mSocket = socket;
    
    mCarUpdateInterval = 0;
    
    for ( unsigned short i = 0; i < 64; i += 1 )
    {
        mRequestedCarInfo[ i ] = mRequestedSessionInfo[ i ] = false;
    }
}

void PluginHandler::CarUpdateArrived ( const short cid )
{
    mLastUpdate[ cid ] = Clock::now ();
}

bool PluginHandler::IsWaitingCarUpdate ( const short cid )
{
    if ( mCarUpdateInterval == 0 )
        return false;
    
    Time t = Clock::now ();
    Ms time_since_update;
    
    time_since_update = std::chrono::duration_cast< Ms > ( t - mLastUpdate[ cid ] );
    
    return ( time_since_update.count () >= mCarUpdateInterval );
}