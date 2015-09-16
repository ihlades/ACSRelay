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
    virtual ~CMDParams () { }
    
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