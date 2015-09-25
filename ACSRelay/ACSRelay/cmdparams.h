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
 * @class CMDParams
 * @brief Helper class that reads the command line paramters
 *        and provides methods to retrieve them.
 */
class CMDParams
{
public:
    
    // CTOR

    /**
     * @brief Constructs a CMDParams object.
     * @param argc Number of command line arguments.
     * @param argv Command line arguments character arrays.
     */
    CMDParams ( int argc, char **argv );
    /**
     * @brief CMDParams destructor.
     */
    virtual ~CMDParams ();
    
    // STRUCTS

    /**
     * @struct PluginParams
     * @brief Holds information about a plugin described in a command line argument.
     */
    struct PluginParams
    {
        std::string name; ///< Plugin identifier (name)
        std::string address; ///< Plugin host address
        unsigned int port; ///< Plugin UDP port
        unsigned int listen_port; ///< Local port on which to listen for packets from the plugin.
    };
    
    // METHODS

    /**
     * @brief Returns the filename of the configuration file.
     * @return Filename of the configuration file or an empty
     *         string if no filename was specified as a
     *         command line argument.
     */
    std::string ConfigFilename () const { return mConfigFilename; }
    /**
     * @brief Returns a list of PluginParams entered as command line arguments.
     * @return List of PluginParams structs.
     */
    std::list < PluginParams > Plugins () const { return mPlugins; }
    /**
     * @brief Returns the remote UDP/TCP port specified as a command line argument.
     * @return Remote UDP/TCP port as an unsigned integer or 0 if none was specified.
     */
    unsigned int RemotePort () const { return mRemotePort; }
    /**
     * @brief Returns the local UDP port specified as a command line argument.
     * @return Local UDP port as an unsigned integer or 0 if none was specified.
     */
    unsigned int LocalPort () const { return mLocalPort; }
    /**
     * @brief Returns the local TCP port specified as a command line argument.
     * @return Local TCP port as an unsigned integer or 0 if none was specified.
     */
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
