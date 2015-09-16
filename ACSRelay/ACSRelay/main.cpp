#include "acsrelay.h"
#include "cmdparams.h"
#include "software.h"

#include <iostream>

int main(int argc, char **argv)
{
    ACSRelay *relay = ACSRelay::Build ();
    CMDParams *params = new CMDParams ( argc, argv );
    
    std::string config_filename;
    
    std::cout << SW_NAME << " version " << SW_VERSION << "." << std::endl;
    
#ifdef DEBUG
    std::cout << "Debug output is enabled." << std::endl;
#endif
    
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
    
    for ( auto p = params -> Plugins().begin (); p != params -> Plugins().end(); ++p )
    {
        relay -> AddPlugin( new PluginHandler ( p -> name, p -> address, p -> listen_port, p -> port ) );
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
    
	return 0;
}
