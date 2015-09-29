#####################################
#                                   #
#      ACSRelay 1.0 readme file     #
#                                   #
#####################################

+-----------+
|  License  |
+-----------+

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

+----------+
| Contents |
+----------+

1. Purpose of ACSRelay

2. Configuration
    2.1. INI configuration file
    2.2. Command line parameters

3. Common usage scenario
    3.1. Scenario overview
    3.2. ACSRelay configuration
    3.3. Configuring server plugins
    3.4. Configuring the game server

4. Changelog

+------------------------+
| 1. Purpose of ACSRelay |
+------------------------+

ACSRelay is a simple software that allows connecting more than one UDP plugin to an Assetto Corsa game server. It does this by orchestrating the communication between the server and the plugins. Towards the server it presents itself as a simple UDP plugin; towards the plugins it presents itself as the server.

If an UDP datagram is sent by the server, ACSRelay will relay it to all the plugins it has been told to send it to. If an UDP datagram is sent by an plugin, ACSRelay will send it unmodified to the server.

If one or more plugins subscribes to car updates or requests various information (e.g.: about the cars, about the session, etc.), then ACSRelay will make sure the server's reponse will be redirected only to the interested plugin(s). This also works when multiple plugins request either the same or different information - ACSRelay will send the update/response packets at the right time to each interested plugin.

+------------------+
| 2. Configuration |
+------------------+

ACSRelay can be configured via two methods: an INI configuration file and command line parameters. The command line parameters overwrite any existing settings from the INI file.

 2.1. INI Configuration file
+---------------------------+

ACSRelay will look for a configuration file named "settings.ini", located in the same folder as the executable, by default. This is a typical INI file with a number of required keys. These are explained in the table below.

      GROUP     |      KEY      |                   VALUE
----------------+---------------+-------------------------------------------
                |               | UDP port on which to listen for packets
                |               | from the AC server.
                |               |
                |               | This is the same as the number after :
                |   RELAY_PORT  | in UDP_PLUGIN_ADDRESS field in the server
                |               | configuration (server_cfg.ini)
                |               |
                |               | * This is a required parameter. If it's missing
                |               |   the relay will not start.
                +---------------+-------------------------------------------
                |               | UDP port on which the server listens.
                |               |
                |               | This is the same number as the one in
                |  SERVER_PORT  | UDP_PLUGIN_LOCAL_PORT field in the
                |               | server configuration (server_cfg.ini)
                |               |
                |               | * This is a required parameter. If it's missing
                |               |   the relay will not start.
                +---------------+-------------------------------------------
                |               | IP address of the server. If the server
                |               | is on the same machine as ACSRelay
     SERVER     |       IP      | then this is usually 127.0.0.1
                |               |
                |               | * Currently only IPv4 addresses are supported.
                |               | * It defaults to 127.0.0.1
                |---------------+-------------------------------------------
                |               | Type of the upstream server. It can have one
                |               | of two values:
                |               |
                |               |  - AC if the upstream server is an
                |               |    Assetto Corsa game server.
                |  SERVER_TYPE  |
                |               |  - RELAY if the upstream server is another
                |               |    instance ACSRelay.
                |               |
                |               | See also:
                |               |       LISTEN_PORT key from RELAY group.
                |               |
                |               | * It defaults to AC
----------------+---------------+-------------------------------------------
                |               | IP address of the plugin. If the plugin
                |               | is on the same machine as ACSRelay
                |       IP      | then this is usually 127.0.0.1
                |               |
                |               | * Currently only IPv4 addresses are supported.
                |               | * It defaults to 127.0.0.1
                +---------------+-------------------------------------------
                |               | UDP port on which the plugin listens
                |               | for data from the server (ACSRelay).
                |               |
                |  REMOTE_PORT  | ACSRelay will send packets to this port.
                |               |
                |               | * It defaults to 0, which will result in
                |               |   undefined behaviour. This port must be
                |               |   configured!
    PLUGIN_#    +---------------+-------------------------------------------
                |               | UDP port on which the
                |               | plugin sends data towards
                |               | the server (ACSRelay)
                |               |
                |               | ACSRelay will listen for packets
                |               | on this port.
                |   LOCAL_PORT  |
                |               | A value of 0 means that ACSRelay will
                |               | ignore packets from this plugin and instead
                |               | will just send server messages to it.
                |               |
                |               | * It defaults to 0, which means the relay
                |               |   won't receive packets from the plugin.
                +---------------+-------------------------------------------
                |               | Name under which the plugin will be
                |      NAME     | identified internally by ACSRelay.
                |               |
                |               | * It defaults to "PLUGIN_#"
----------------+---------------+-------------------------------------------
                |               | TCP port on which ACSRelay will listen for
                |               | connections from other (downstream) ACSRelays.
                |               |
                |               | The downstream ACSRelays will act as plugins,
      RELAY     |   LISTEN_PORT | while this instance of ACSRelay will act as a
                |               | server.
                |               |
                |               | * It defaults to 0, which means this feature
                |               |   is disabled.

There can be multiple PLUGIN_# groups, where the suffix (marked by the hash signed) will be a different number. The group's title is used to identify the specific plugin. An example of a configuration file could be the following:

# start of configuration file

[SERVER]
RELAY_PORT=9999
SERVER_PORT=9999

[PLUGIN_0]
RELAY_PORT=9550
PLUGIN_PORT=9555

[PLUGIN_1]
NAME=Plugin name
IP=192.168.1.2
RELAY_PORT=0
PLUGIN_PORT=9666

# end of configuration file

The first plugin would work on the same machine as the ACSRelay. This plugin would be configured to listen for messages from the server on port 9555 and send messages to the server on port 9550. The second plugin would work on a remote machine with the IP address of 192.168.1.2; this plugin would receive messages from the server (via ACSRelay) but ACSRelay will discard any packets originating from this plugin (because RELAY_PORT=0).

 2.2. Command line parameters
+----------------------------+

There are several command line parameters that ACSRelay reads. They all have priority over conflicting settings found in the INI file. A table containing the available command line parameters can be found below.


                PARAMETER               |                VALUE
----------------------------------------+------------------------------------
                                        | UDP port on which ACSRelay will
            -lp <LOCAL_PORT>            | listen for messages from the
                                        | server.
----------------------------------------+
                                        | Examples:
       --local-port <LOCAL_PORT>        |               -lp 9999
                                        |               --local-port 65022
----------------------------------------+------------------------------------
                                        | UDP port on which ACSRelay will
           -sp <REMOTE_PORT>            | send messages towards the
                                        | server.
----------------------------------------+
                                        | Examples:
      --server-port <REMOTE_PORT>       |               -sp 9999
                                        |               --server-port 65022
----------------------------------------+------------------------------------
                                        | TCP port on which ACSRelay will
            -rp <RELAY_PORT>            | listen for connections from
                                        | other (downstream) ACSRelays.
                                        | The downstream ACSRelays will act
----------------------------------------+ as plugins, while this instance
                                        | will have the role of a server
                                        |
        --relay-port <RELAY_PORT>       | Examples:
                                        |               -rp 9999
                                        |               --relay-port 65022
----------------------------------------+------------------------------------
                                        | Location of the INI file
            -c <CONFIG_FILE>            | to be used.
                                        |
----------------------------------------+ Example:
                                        |           -c "acsrelay.ini"
       --config-file <CONFIG_FILE>      |           --config-file "settings.ini"
                                        |
----------------------------------------+------------------------------------
                                        | Adds a plugin with the specified
                                        | parameters. Can be used multiple
                 -p                     | times.
  <NAME>:<ADDRESS>:<OUT_PORT>:<IN_PORT> |
                                        | NAME:     Plugin identifier
                                        | ADDRESS:  IP address of plugin
----------------------------------------+ OUT_PORT: Same as plugin OUT_PORT
                                        |               in INI file
                                        | IN_PORT:  Same as IN_PORT
             --add-plugin               |               in INI file
  <NAME>:<ADDRESS>:<OUT_PORT>:<IN_PORT> |
                                        | Examples:
                                        |       -P MR:127.0.0.1:9555:9666
                                        |       --plugin PLUGIN_1:10.0.0.1:1234:5678

+--------------------------+
| 3. Common usage scenario |
+--------------------------+

The most common usage scenario will have all the software running on the same machine. That is the Assetto Corsa Dedicated Server, any UDP plugins (e.g.: Minorating) and ACSRelay will all be installed on the same computer. In this case, it is important to know that two applications cannot share the same UDP port (inbound and outbound ports are treated separately), so this will be the focus of our configuration.

 3.1. Scenario overview
+----------------------+

The following diagram presents an overview of our application:



                                                  ----> MinoRating
                                                 /
                                                /
 Assetto Corsa                                 /
                    <---->   ACSRelay   <-----<
Dedicated Server                               \
                                                \
                                                 \
                                                  ----> sTracker


The ACSRelay software sits between the Assetto Corsa Dedicated Server the MinoRating and sTracker plugins, having the role of orchestrating the communication between these parties. Note that communication only happens between the server and one or more of the plugins, but the plugins cannot exchange messages via ACSRelay. Any inter-plugin communication must be handled in another way - either via the game server (e.g.: one plugin sends a chat message, the server notifies other plugins about the message) or by implementing this feature in the plugins themselves.

It is important that the server, ACSRelay and the plugins use different UDP ports. For this example we will configure the server to listen on port 9000, ACSRelay will listen for messages from the server on port 9001 and from the plugins on ports 9010 and 9020. The plugins will listen on ports 9011 and 9021.

The configuration is done in three steps:
    1. Configuring ACSRelay
    2. Configuring the UDP plugins
    3. Configuring the Assetto Corsa server


 3.2. ACSRelay configuration
+---------------------------+

The following is the required INI configuration file for ACSRelay in order to make the topology mentioned above:

Start of settings.ini
+------------------------------------------------------------------------------------------------------------------------------

# AC Server configuration
[SERVER]
SERVER_PORT=9000 # Server UDP port (same as "UDP_PLUGIN_LOCAL_PORT" in server_cfg.ini)
RELAY_PORT=9001  # ACSRelay UDP port (same as the port number after ':' from the "UDP_PLUGIN_ADDRESS" in server_cfg.ini)

# MinoRating plugin configuration
[PLUGIN_0]
NAME=MinoRating
RELAY_PORT=9010  # ACSRelay UDP port (same as "ac_server_port" from MinoRating configuration)
PLUGIN_PORT=9011 # MinoRating plugin port (same as "plugin_port" from MinoRating configuration)

# sTracker plugin configuration
[PLUGIN_1]
NAME=sTracker
RELAY_PORT=9020  # ACSRelay UDP port (same as "sendPort" from [ACPLUGIN] in sTracker configuration)
PLUGIN_PORT=9021 # sTracker UDP port (same as "rcvPort" from [ACPLUGIN] in sTracker configuration)

+------------------------------------------------------------------------------------------------------------------------------
End of settings.ini

 3.3. Configuring server plugins
+-------------------------------+

Each plugin must be configured to use the ports corresponding with the ACSRelay configuration. Only relevant lines in the plugin configurations will be shown. Say, for example, we want to configure MinoRating to work with ACSRelay in the topology presented above. The plugin will send data on port 9010 and receive data on port 9011. As such, the MinoRating configuration file will have to have the following lines:

Start of MinoRating.exe.config
+-----------------------------------------------------------------------------------------------------------------------------

<!-- AcServer/proxy listening port like defined in UDP_PLUGIN_LOCAL_PORT -->
<add key="ac_server_port" value="9010" />
<!-- Plugin listening port towards acServer/proxy like defined in UDP_PLUGIN_ADDRESS=12000 (without 127.0.0.1, port number only) -->
<add key="plugin_port" value="9011" />

+-----------------------------------------------------------------------------------------------------------------------------
End of MinoRating.exe.config

The second plugin (sTracker) would be configured to listen on port 9021 and send data on port 9020. The configuration file "sTracker.ini" will contain the following:

Start of sTracker.ini
+------------------------------------------------------------------------------------------------------------------------------

[ACPLUGIN]
; udp port the plugins receives from. -1 means to use the AC servers setting UDP_PLUGIN_ADDRESS
rcvPort = 9021
; udp port the plugins sends to. -1 means to use the AC servers setting UDP_PLUGIN_LOCAL_PORT
sendPort = 9020

+-----------------------------------------------------------------------------------------------------------------------------
End of sTracker.ini

Note that the [ACPLUGIN] group is usually found near the end of the configuration file for sTracker.

3.4. Configuring the game server
+-------------------------------+

The Assetto Corsa Dedicated Server must be configured to exchange messages with ACSRelay. The game server does not have any clue of the existence of multiple plugins, it must be configured to communicate with just one party. As ACSRelay runs on the same machine as the game server, its IP address will be 127.0.0.1. ACSRelay was configured to listen on port 9001 so the full address will be 127.0.0.1:9001, while the server will listen on UDP port 9000.

The relevant lines in the server's configuration file (server_cfg.ini) are listed below:

UDP_PLUGIN_ADDRESS=127.0.0.1:9001
UDP_PLUGIN_LOCAL_PORT=9000

+--------------+
| 4. Changelog |
+--------------+

1.0: Initial release