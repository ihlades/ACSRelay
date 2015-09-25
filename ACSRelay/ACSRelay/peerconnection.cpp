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
