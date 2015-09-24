#include "acsrelay.h"
#include "cmdparams.h"
#include "software.h"
#include "log.h"

#include <iostream>

int main(int argc, char **argv)
{
    ACSRelay *relay = ACSRelay::Build ();
    CMDParams *params = new CMDParams ( argc, argv );
    std::list <CMDParams::PluginParams> plugins;
    
    std::string config_filename;
    
#ifdef _DEBUG
    Log::Start ( Log::DEBUG, Log::DEFAULT_LOG_FILE );
#else
    Log::Start ( Log::NORMAL, Log::DEFAULT_LOG_FILE );
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
