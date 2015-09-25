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
#include "cmdparams.h"
#include "software.h"
#include "log.h"

#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
#endif

int main(int argc, char **argv)
{
    ACSRelay *relay = ACSRelay::Build ();
    CMDParams *params = new CMDParams ( argc, argv );
    std::list <CMDParams::PluginParams> plugins;
    
    std::string config_filename;

#ifdef _WIN32
    WSADATA wsadata;
    WSAStartup ( MAKEWORD ( 2, 2 ), &wsadata );
#endif
    
#ifdef _DEBUG
    Log::Start ( Log::DEBUG_LVL, Log::DEFAULT_LOG_FILE );
#else
    Log::Start ( Log::NORMAL_LVL, Log::DEFAULT_LOG_FILE );
#endif
    
    Log::i() << SW_NAME << " version " << SW_VERSION << ".";
    Log::d() << "Debug output is enabled.";
    
    // Determine configuration file name. The default value is declared in "software.h".
    
    if ( params -> ConfigFilename() != "" )
    {
        config_filename = params -> ConfigFilename();
    }
    else
    {
        config_filename = DEFAULT_CFG_FILE;
    }

    relay -> ReadConfiguration ( config_filename );
    
    // Add plugins from command line parameters.
    
    plugins = params -> Plugins();
    
    for ( auto p = plugins.begin (); p != plugins.end(); ++p )
    {
        relay -> AddPeer( new PeerConnection ( p -> name, p -> address, p -> listen_port, p -> port ) );
    }
    
    if ( params -> RemotePort() != 0 )
    {
        relay -> SetRemotePort ( params -> RemotePort () );
    }
     
    if ( params -> LocalPort() != 0 )
    {
        relay -> SetLocalPort ( params -> LocalPort () );
    }
     
    if ( params -> RelayPort() != 0 )
    {
        relay -> SetRelayPort ( params -> RelayPort () );
    }
    
    delete params;
    
    relay -> Start ();
    
    delete relay;
    
    Log::Stop();
    
    return 0;
}
