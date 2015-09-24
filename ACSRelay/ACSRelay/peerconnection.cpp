#include "peerconnection.h"

#include "udpsocket.h"

PeerConnection::PeerConnection ( const std::string name, const std::string host, const unsigned int local_port, const unsigned int remote_port )
{
    mName = name;
    mSocket = new UDPSocket ( host, local_port, remote_port );
    
    mCarUpdateInterval = 0;
    
    for ( unsigned short i = 0; i < 64; i += 1 )
    {
        mRequestedCarInfo[ i ] = mRequestedSessionInfo[ i ] = false;
    }
}

PeerConnection::PeerConnection ( const std::string name, Socket* socket )
{
    mName = name;
    mSocket = socket;
    
    mCarUpdateInterval = 0;
    
    for ( unsigned short i = 0; i < 64; i += 1 )
    {
        mRequestedCarInfo[ i ] = mRequestedSessionInfo[ i ] = false;
    }
}

void PeerConnection::CarUpdateArrived ( const short cid, Time time )
{
    mLastUpdate[ cid ] = time;
}

bool PeerConnection::IsWaitingCarUpdate ( const short cid, Time time )
{
    if ( mCarUpdateInterval == 0 )
        return false;
    
    Ms time_since_update;
    
    time_since_update = std::chrono::duration_cast< Ms > ( time - mLastUpdate[ cid ] );
    
    return ( time_since_update.count () >= mCarUpdateInterval );
}

PeerConnection::~PeerConnection()
{
    delete mSocket;
}
