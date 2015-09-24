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

#ifndef __ACS_PROTOCOL_H
#define __ACS_PROTOCOL_H

/**
 * @brief Contains most used constants in the Assetto Corsa Server UDP protocol,
 *        namely the types of packets.
 */
namespace ACSProtocol
{
    const char ACSP_NEW_SESSION = 50;
    const char ACSP_NEW_CONNECTION = 51;
    const char ACSP_CONNECTION_CLOSED = 52;
    const char ACSP_CAR_UPDATE = 53;
    const char ACSP_CAR_INFO = 54; // Sent as response to ACSP_GET_CAR_INFO command
    const char ACSP_END_SESSION = 55;
    const char ACSP_VERSION = 56;
    const char ACSP_CHAT = 57;
    const char ACSP_CLIENT_LOADED = 58;
    const char ACSP_SESSION_INFO = 59;
    const char ACSP_ERROR = 60;
    const char ACSP_LAP_COMPLETED = 73;
    
    // EVENTS
    const char ACSP_CLIENT_EVENT = 130;
    
    // EVENT TYPES
    const char ACSP_CE_COLLISION_WITH_CAR = 10;
    const char ACSP_CE_COLLISION_WITH_ENV = 11;
    
    // COMMANDS
    const char ACSP_REALTIMEPOS_INTERVAL = 200;
    const char ACSP_GET_CAR_INFO = 201;
    const char ACSP_SEND_CHAT = 202; // Sends chat to one car
    const char ACSP_BROADCAST_CHAT = 203; // Sends chat to everybody
    const char ACSP_GET_SESSION_INFO = 204;
    const char ACSP_SET_SESSION_INFO = 205;
    const char ACSP_KICK_USER = 206;
}

#endif // __ACS_PROTOCOL_H
