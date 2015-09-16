#####################################
#                                   #
#      ACSRelay 0.0 readme file     #
#                                   #
#####################################

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
                |   LOCAL_PORT  |
                |               | This is the same as the number after :
                |               | in UDP_PLUGIN_ADDRESS field in the server
     SERVER     |               | configuration (server_cfg.ini)
                +---------------+-------------------------------------------
                |               | UDP port on which the server listens.
                |               |
                |  REMOTE_PORT  | This is the same number as the one in
                |               | UDP_PLUGIN_LOCAL_PORT field in the
                |               | server configuration (server_cfg.ini)
----------------+---------------+-------------------------------------------
                |               | IP address of plugin. If the plugin
                |       IP      | is on the same machine as ACSRelay
                |               | then this is usually 127.0.0.1
                +---------------+-------------------------------------------
                |               | UDP port on which the plugin listens
                |  REMOTE_PORT  | for data from the server (ACSRelay).
                |               |
                |               | ACSRelay will send packets to this port.
                +---------------+-------------------------------------------
    PLUGIN_#    |               | UDP port on which the
                |               | plugin sends data towards
                |               | the server (ACSRelay)
                |               |
                |               | ACSRelay will listen for packets
                |   LOCAL_PORT  | on this port.
                |               |
                |               | A value of 0 means that ACSRelay will
                |               | ignore packets from this plugin and instead
                |               | will just send server messages to it.

There can be multiple PLUGIN_# groups, where the suffix (marked by the hash signed) will be a different number. The group's title is used to identify the specific plugin. An example of a configuration file could be the following:

# start of configuration file

[SERVER]
LOCAL_PORT=9999
REMOTE_PORT=9999

[PLUGIN_0]
IP=192.168.1.103
LOCAL_PORT=9555
REMOTE_PORT=9555

[PLUGIN_1]
IP=127.0.0.1
LOCAL_PORT=0
REMOTE_PORT=9666

# end of configuration file

The first plugin would work on a remote machine, identified by the IP address of 192.168.1.103. This plugin would be configured to listen for messages from the server on port 9555 and send messages to the server on port 9555. The second plugin would work on the same machine as ACSRelay (IP address of 127.0.0.1); this plugin will receive messages from the server (via ACSRelay) but ACSRelay will discard any packets originating from this plugin.

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
           -rp <REMOTE_PORT>            | send messages towards the
                                        | server.
----------------------------------------+
                                        | Examples:
      --remote-port <REMOTE_PORT>       |               -rp 9999
                                        |               --remote-port 65022
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
              --plugin                  |               in INI file
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



                                                  ----> UDP Plugin 1
                                                 /
                                                /
 Assetto Corsa                                 /
                    <---->   ACSRelay   <-----------> UDP Plugin 2
Dedicated Server                               \
                                                \
                                                 \
                                                  ----> UDP Plugin 3


The ACSRelay software sits between the Assetto Corsa Dedicated Server and any number of UDP Plugins, having the role of orchestrating the communication between these two parties. Note that communication only happens between the server and one or more of the plugins, but the plugins cannot exchange messages via ACSRelay. Any inter-plugin communication must be handled in another way - either via the game server (e.g.: one plugin sends a chat message, the server notifies other plugins about the message) or by implementing this feature in the plugins themselves.

It is important that the server, ACSRelay and the plugins use different UDP ports. For this example we will configure the server to listen on port 9500, ACSRelay will listen for messages from the server on port 9600 and from the plugins on ports 9601, 9602 and 9603. The plugins will listen on ports 9700, 9800 and 9900.

The configuration is done in three steps:
    1. Configuring ACSRelay
    2. Configuring the UDP plugins
    3. Configuring the Assetto Corsa server


 3.2. ACSRelay configuration
+---------------------------+

The following is the required INI configuration file for ACSRelay in order to make the topology mentioned above:

# start of configuration file

[SERVER]
LOCAL_PORT=9600
REMOTE_PORT=9500

[PLUGIN_0]
IP=127.0.0.1
LOCAL_PORT=9601
REMOTE_PORT=9700

[PLUGIN_1]
IP=127.0.0.1
LOCAL_PORT=9602
REMOTE_PORT=9800

[PLUGIN_2]
IP=127.0.0.1
LOCAL_PORT=9603
REMOTE_PORT=9900

# end of configuration file

 3.3. Configuring server plugins
+-------------------------------+

Each plugin must be configured to use the ports corresponding with the ACSRelay configuration. Say, for example, the first plugin (PLUGIN_0 in ACSRelay's configuration) is the ferrito-liveCarTracker plugin. We would edit its configuration file to listen on port 9700 and send data on port 9601, as shown below. Note that only the relevant part of the configuration file is listed:

<configuration>
    <appSettings>
        <add key="udpPort" value="9700" />
        <add key="acServerIPAddress" value="127.0.0.1" />
        <add key="acServerIPPort" value="9601" />
    </appSettings>
</configuration>

The second plugin (PLUGIN_1) would be configured to listen on port 9800 and send data on port 9602, while the third plugin (PLUGIN_2) would listen on 9900 and send packets on 9603.


3.4. Configuring the game server
+-------------------------------+

The Assetto Corsa Dedicated Server must be configured to exchange messages with ACSRelay. The game server does not have any clue of the existence of multiple plugins, it must be configured to communicate with just one party. As ACSRelay runs on the same machine as the game server, its IP address will be 127.0.0.1. ACSRelay was configured to listen on port 9600 so the full address will be 127.0.0.1:9600, while the server will listen on UDP port 9500.

The relevant lines in the server's configuration file (server_cfg.ini) are listed below:

UDP_PLUGIN_ADDRESS=127.0.0.1:9600
UDP_PLUGIN_LOCAL_PORT=9500

+--------------+
| 4. Changelog |
+--------------+

TBD