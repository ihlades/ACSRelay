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
#include "configuration.h"
#include "software.h"
#include "log.h"

#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
#endif

int main(int argc, char **argv)
{
    ACSRelay *relay;
    Configuration *config = new Configuration ();

    config -> ReadArguments ( argc, argv );
    config -> ReadSettingsFile ();

    relay = ACSRelay::Build ( config -> RelayConfiguration() );

#ifdef _WIN32
    WSADATA wsadata;
    WSAStartup ( MAKEWORD ( 2, 2 ), &wsadata );
#endif

    Log::Start ( config -> LogLevel(), config -> LogFile() );
    
    Log::i() << SW_NAME << " version " << SW_VERSION << ".";
    Log::d() << "Debug output is enabled.";

    //    Log::v () << "Using configuration file \"" << params -> ConfigFilename() << "\"";
    
    delete config;
    
    relay -> Start ();
    
    delete relay;
    
    Log::Stop();
    
    return 0;
}
