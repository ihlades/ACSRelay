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

#include "configuration.h"
#include "INIReader.h"
#include "software.h"
#include "log.h"

#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

Configuration::Configuration ()
	: mConfigFilename(DEFAULT_CFG_FILE),
      mRelay {"127.0.0.1", 0, 0, 0, AUTO},
#ifdef _DEBUG
      mLogLevel(Log::DEBUG_LVL)
#else
      mLogLevel(Log::NORMAL_LVL)
#endif
{
}

Configuration::~Configuration()
{
}

void Configuration::ReadParameters ( int argc, char **argv )
{
    extern char* optarg;
    extern int opterr, optopt;
    int c;
    int opt;

    opterr = 0;

    static struct option log_options[] {
        {"no-log-file",     required_argument,  0,  1 },
        {"log-file",        required_argument,  0,  'l' },
        {"verbose",         no_argument,        0,  'v' },
        {0,                 0,                  0,  0 }
    };

    static struct option long_options[] = {
        {"local-port",      required_argument,  0,  0 },
        {"lp",              required_argument,  0,  0 },
        {"server-port",     required_argument,  0,  0 },
        {"sp",              required_argument,  0,  0 },
        {"relay-port",      required_argument,  0,  0 },
        {"rp",              required_argument,  0,  0 },
        {"add-plugin",      required_argument,  0,  'p' },
        {"config-file",     required_argument,  0,  'c' },
        {"no-log-file",     required_argument,  0,  1 }, // Duplicate these three longopts
        {"log-file",        required_argument,  0,  'l' }, // to avoid having getopt_long()
        {"verbose",         no_argument,        0,  'v' }, // say they're unexpected.
        {0,                 0,                  0,  0 }
    };

    // Parse Log settings first so we can start using the logger.

    while ( ( c = getopt_long_only ( argc, argv, "v", log_options, &opt ) ) != -1 )
    {
        switch ( int(c) )
        {
            case 1:
                Log::SetOutputFile("");
                break;
            case 'l':
                if ( !optarg )
                {
                    break;
                }
                else if ( strlen ( optarg ) == 0 )
                {
                    Log::SetOutputFile("");
                }
                else
                {
                    Log::SetOutputFile(optarg);
                }
                break;
            case 'v':
                if ( mLogLevel < Log::VERBOSE_LVL )
                    Log::SetOutputLevel( Log::VERBOSE_LVL );
                break;
        }
    }

    optind = 1;

    while ( ( c = getopt_long_only ( argc, argv, ":c:p:", long_options, &opt ) ) != -1 )
    {
        switch ( int(c) )
        {
            case 0:
            {
                // Long option
                switch ( optind )
                {
                    case 0:
                    case 1:
                        if ( !optarg )
                        {
                            Log::e() << "Argument missing for option --" << long_options[ opt ].name;
                            break;
                        }
                        mRelay.local_port = atoi ( optarg );
                        break;
                    case 2:
                    case 3:
                        if ( !optarg )
                        {
                            Log::e() << "Argument missing for option --" << long_options[ opt ].name;
                            break;
                        }
                        mRelay.remote_port = atoi ( optarg );
                        break;
                    case 4:
                    case 5:
                        if ( !optarg )
                        {
                            Log::e() << "Argument missing for option --" << long_options[ opt ].name;
                            break;
                        }
                        mRelay.relay_port = atoi ( optarg );
                        break;
                }
                break;
            }

            case 'c':
                mConfigFilename = optarg;
                break;
            case 'p':
                mRelay.plugins.push_back( PluginParamsFromString ( optarg ) );
                break;
            case 'v':
                if ( mLogLevel < Log::VERBOSE_LVL )
                    Log::SetOutputLevel( Log::VERBOSE_LVL );
                break;
            case ':':
                Log::e() << "Argument missing for option '-" << static_cast<char> (optopt) << "'";
                break;
            case '?':
                break;
        }
    }
}

void Configuration::ReadSettingsFile ()
{
    std::vector< std::string > sections;
    INIReader *ir = new INIReader ();

    ir -> parse ( mConfigFilename );

    if ( mRelay.local_port == 0 )
        mRelay.local_port = static_cast<unsigned int> ( ir -> GetInteger ( "SERVER", "RELAY_PORT", 0 ) );

    if ( mRelay.remote_port == 0 )
        mRelay.remote_port = static_cast<unsigned int> ( ir -> GetInteger( "SERVER", "SERVER_PORT", 0 ) );

    if ( mRelay.server_type == AUTO )
        mRelay.server_type = ir -> GetString ( "SERVER", "TYPE", "AC" ) == "RELAY" ? RELAY : AC;

    if ( mRelay.host == "" )
        mRelay.host = ir -> GetString ( "SERVER", "IP", "127.0.0.1" );

    if ( mRelay.relay_port == 0 )
        mRelay.relay_port = static_cast<unsigned int> ( ir -> GetInteger ( "RELAY", "LISTEN_PORT", 0 ) );

    sections = ir -> Sections ();

    for ( unsigned int i = 0; i < sections.size (); i += 1 )
    {
        if ( sections[ i ].substr ( 0, 7 ) == "PLUGIN_" )
        {
            // ADD NEW PLUGIN

            mRelay.plugins.push_back(
               PluginParams {
                   ir -> GetString ( sections[ i ], "NAME", sections[ i ] ),
                   ir -> GetString ( sections[ i ], "IP", "127.0.0.1" ),
                   static_cast<unsigned int> ( ir -> GetInteger ( sections[ i ], "PLUGIN_PORT", 0 ) ),
                   static_cast<unsigned int> ( ir -> GetInteger ( sections[ i ], "RELAY_PORT", 0 ) )
               }
            );
        }
    }
}

Configuration::PluginParams Configuration::PluginParamsFromString ( const char *s )
{
    PluginParams params;
    char *p, str[256];

    params.name = params.host = "";
    params.remote_port = params.local_port = 0;

    strncpy ( str, s, sizeof(str) - 1 );
    str[ sizeof(str) - 1 ] = '\0';	// Make sure str is terminated (strncpy() doesn't ensure this)

    p = strtok( str, ":");

    while ( p != NULL )
    {
        if ( params.name == "" )
        {
            params.name = p;
        }
        else if ( params.host == "" )
        {
            params.host = p;
        }
        else if ( params.remote_port == 0 )
        {
            params.remote_port = atoi ( p );
        }
        else if ( params.local_port == 0 )
        {
            params.local_port = atoi ( p );
        }
        else
        {
            // INCORRECT SYNTAX
        }

        p = strtok( NULL, ":" );
    }

    return params;
}
