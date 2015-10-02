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

#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

#include <string>
#include <list>

#include "log.h"

/**
 * @class Configuration
 * @brief Helper class to read configuration from INI settings file and command line arguments.
 */
class Configuration
{
public:
    
    // CTOR

    /**
     * @brief Constructs a CMDParams object.
     */
    Configuration ();
    /**
     * @brief CMDParams destructor.
     */
    virtual ~Configuration ();
    
    // STRUCTS

    /**
     * @struct PluginParams
     * @brief Holds information about a plugin described in a command line argument.
     */
    struct PluginParams
    {
        std::string name; ///< Plugin identifier (name)
        std::string host; ///< Plugin host address
        unsigned int remote_port; ///< Plugin UDP port
        unsigned int local_port; ///< Local port on which to listen for packets from the plugin.
    };

    enum ServerType
    {
        AUTO,
        AC,
        RELAY
    };

    struct RelayParams
    {
        std::string host;
        unsigned int local_port;
        unsigned int remote_port;
        unsigned int relay_port;
        ServerType server_type;
        std::list<PluginParams> plugins;
    };
    
    // METHODS

    /**
     * @brief Reads command line arguments.
     * @param argc Number of command line arguments.
     * @param argv Command line arguments character arrays.
     */
    void ReadParameters ( int argc, char **argv );
    /**
     * @brief Reads configuration from the INI settings file.
     */
    void ReadSettingsFile ();

    /**
     * @brief Returns the filename of the configuration file.
     * @return Filename of the configuration file or an empty
     *         string if no filename was specified as a
     *         command line argument.
     */
    std::string ConfigFilename () const { return mConfigFilename; }
    /**
     * @brief Used to retrieve relay configuration, computed from settings file and command line parameters.
     * @return RelayParams structure.
     */
    RelayParams RelayConfiguration () const { return mRelay; }
    /**
     * @brief Returns a list of PluginParams entered as command line arguments.
     * @return List of PluginParams structs.
     */
    std::list < PluginParams > Plugins () const { return mRelay.plugins; }
    /**
     * @brief Returns the remote UDP/TCP port specified as a command line argument.
     * @return Remote UDP/TCP port as an unsigned integer or 0 if none was specified.
     */
    unsigned int RemotePort () const { return mRelay.remote_port; }
    /**
     * @brief Returns the local UDP port specified as a command line argument.
     * @return Local UDP port as an unsigned integer or 0 if none was specified.
     */
    unsigned int LocalPort () const { return mRelay.local_port; }
    /**
     * @brief Returns the local TCP port specified as a command line argument.
     * @return Local TCP port as an unsigned integer or 0 if none was specified.
     */
    unsigned int RelayPort () const { return mRelay.relay_port; }
    /**
     * @brief Used to retrieve the log file name.
     * If the log file name is not set via command line parameters,
     * the default value is returned.
     * @return Log file name as a string.
     */
    std::string LogFile () const { return mLogFile; }
    /**
     * @brief Used to retrieve the log output level.
     * If the log output level is not set via command line parameters,
     * the default value is returned.
     * @return Log output level as a Log::OutputLevel
     */
    Log::OutputLevel LogLevel () const { return mLogLevel; }
    
private:
    
    struct PluginParams PluginParamsFromString ( const char *s );
    
    std::string mConfigFilename;
    RelayParams mRelay;

    std::string mLogFile;

    Log::OutputLevel mLogLevel;
};

#endif // __CONFIGURATION_H
