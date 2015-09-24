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

#ifndef __CMDPARAMS_H
#define __CMDPARAMS_H

#include <string>
#include <list>

/**
 * @brief Helper class that reads the command line paramters
 *        and provides methods to retrieve them.
 */
class CMDParams
{
public:
    
    // CTOR
    
    CMDParams ( int argc, char **argv );
    virtual ~CMDParams ();
    
    // STRUCTS
    
    struct PluginParams
    {
        std::string name;
        std::string address;
        unsigned int port;
        unsigned int listen_port;
    };
    
    // METHODS
    
    std::string ConfigFilename () const { return mConfigFilename; }
    std::list < PluginParams > Plugins () const { return mPlugins; }
    unsigned int RemotePort () const { return mRemotePort; }
    unsigned int LocalPort () const { return mLocalPort; }
    unsigned int RelayPort () const { return mRelayPort; }
    
private:
    CMDParams ();
    
    struct PluginParams PluginParamsFromString ( const char *s );
    
    std::string mConfigFilename;
    std::list <PluginParams> mPlugins;
    unsigned int mLocalPort;
    unsigned int mRemotePort;
    unsigned int mRelayPort;
};

#endif // __CMDPARAMS_H
