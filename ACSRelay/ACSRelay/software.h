#ifndef __SOFTWARE_H
#define __SOFTWARE_H

#define SW_NAME "ACSRelay"
#define SW_DESCRIPTION "Assetto Corsa server plugin relay. Use it to connect more than one UDP plugin to an Assetto Corsa game server."

#define RAW_VERSION "1.0"

#ifdef DEBUG
    #define SW_VERSION RAW_VERSION" (debug)"
#else
    #define SW_VERSION RAW_VERSION
#endif

#define DEFAULT_CFG_FILE "settings.ini"

#endif // __SOFTWARE_H
