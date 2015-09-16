#include <string>

#include <INIReader.h>
#include <pluginhandler.h>
#include <ACSProtocol.h>
#include <Socket.h>

#ifdef DEBUG
    #include <fstream>
#endif

#define BUFFER_SIZE 512

/**
 * @brief Main class whose instance handles the messages exchange between
 *        the Assetto Corsa Server and the various UDP plugins. It establishes
 *        the connection to the server and initiates communication with the
 *        plugins defined in the configuration file. It then relays messages
 *        from the server to the UDP plugins and the other way around.
 */
class ACSRelay
{
public:
    // CTOR
    
    /**
     * @brief Returns pointer to the ACSRelay instance.
     * It creates the ACSRelay object if none exists.
     */
    static ACSRelay* Build ();
    virtual ~ACSRelay () {}
    
    // METHODS
    
    /**
     * @brief Reads configuration from the specified INI file.
     * @param ini_fn Name of file containing configuration options in INI format.
     */
    void ReadConfiguration ( std::string ini_fn );
    /**
     * @brief Adds a plugin in the specified list.
     * @param plugin Pointer to a PluginHandler object associated with a plugin.
     */
    void AddPlugin ( PluginHandler *plugin );
    /**
     * @brief Monitors traffic between AC Server and UDP plugins.
     */
    void Start ();
    
private:
    
    // CTOR
    
    /**
     * @brief ACSRelay object constructor.
     */
    ACSRelay ();
    
    // METHODS
    
    /**
     * @brief Reads, interprets and relays datagrams coming from an UDP plugin.
     * @param plugin Pointer to a Plugin object, associated with the UDP plugin that generated the datagram.
     */
    void RelayFromPlugin ( PluginHandler* plugin );
    /**
     * @brief Reads, interprets and relays datagrams coming from the AC Server.
     */
    void RelayFromServer ();
    
#ifdef DEBUG
    void OutputString ( const std::string msg );
    void OutputPacket ( const char* msg, long n );
#endif
    
    // VARS
    
    static ACSRelay* mInstance;
    
    unsigned int mLocalPort;
    unsigned int mRemotePort;
    int mMaxFd;
    Socket* mServerSocket;
    
    std::map< int, PluginHandler* > mPlugins;
    
    unsigned long mMri;
    
#ifdef DEBUG
    std::ofstream dOut;
#endif
};
