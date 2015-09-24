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
