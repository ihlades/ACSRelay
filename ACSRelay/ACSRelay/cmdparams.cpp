#include "cmdparams.h"

#include <iostream>
#include <getopt.h>
#include <stdlib.h>

CMDParams::CMDParams ( int argc, char **argv )
{
    extern char* optarg;
    int c;
    int optind;
    
    static struct option long_options[] = {
        {"local-port",  required_argument, 0,  1 },
        {"lp",          required_argument, 0,  1 },
        {"server-port", required_argument, 0,  2 },
        {"sp",          required_argument, 0,  2 },
        {"relay-port",  required_argument, 0,  3 },
        {"rp",          required_argument, 0,  3 },
        {"add-plugin",  required_argument, 0,  4 },
        {"config-file", required_argument, 0,  5 },
        {0,         0,                 0,  0 }
    };
    
    mConfigFilename = "";
    mLocalPort = mRemotePort = 0;
    mPlugins.clear();
    
    while ( ( c = getopt_long_only ( argc, argv, "c:p:", long_options, &optind ) ) != -1 )
    {
        switch ( c )
        {
            case 0:
            {
                // Long option
                switch ( optind )
                {
                    case 1:
                        if ( !optarg )
                        {
                            std::cerr << "Argument missing for option --" << long_options[ optind ].name << std::endl;
                            break;
                        }
                        mLocalPort = atoi ( optarg );
                        break;
                    case 2:
                        if ( !optarg )
                        {
                            std::cerr << "Argument missing for option --" << long_options[ optind ].name << std::endl;
                            break;
                        }
                        mRemotePort = atoi ( optarg );
                        break;
                    case 3:
                        if ( !optarg )
                        {
                            std::cerr << "Argument missing for option --" << long_options[ optind ].name << std::endl;
                            break;
                        }
                        mRelayPort = atoi ( optarg );
                        break;
                    case 4:
                        if ( !optarg )
                        {
                            std::cerr << "Argument missing for option --" << long_options[ optind ].name << std::endl;
                            break;
                        }
                        mPlugins.push_back( PluginParamsFromString ( optarg ) );
                        break;
                    case 5:
                        if ( !optarg )
                        {
                            std::cerr << "Argument missing for option --" << long_options[ optind ].name << std::endl;
                            break;
                        }
                        mConfigFilename = optarg;
                        break;
                }
                break;
            }
                
            case 'c':
                if ( !optarg )
                {
                    std::cerr << "Argument missing for option '-c'" << std::endl;
                    break;
                }
                mConfigFilename = optarg;
                break;
            case 'p':
                if ( !optarg )
                {
                    std::cerr << "Argument missing for option '-p'" << std::endl;
                    break;
                }
                mPlugins.push_back( PluginParamsFromString ( optarg ) );
                break;
        }
    }
}

CMDParams::PluginParams CMDParams::PluginParamsFromString ( const char *s )
{
    PluginParams params;
    char *p, str[256];
    
    params.name = params.address = "";
    params.port = params.listen_port = 0;
    
    strncpy ( str, s, 256 );
    
    p = strtok( str, ":");
    
    while ( p != NULL )
    {
        if ( params.name == "" )
        {
            params.name = p;
        }
        else if ( params.address == "" )
        {
            params.address = p;
        }
        else if ( params.port == 0 )
        {
            params.port = atoi ( p );
        }
        else if ( params.listen_port == 0 )
        {
            params.listen_port = atoi ( p );
        }
        else
        {
            // INCORRECT SYNTAX
        }
        
        p = strtok( p, ":" );
    }
    
    return params;
}