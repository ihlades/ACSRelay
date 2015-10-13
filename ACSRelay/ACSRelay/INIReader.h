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

#ifndef _inireader_h
#define _inireader_h

#include <string>
#include <map>
#include <vector>

typedef std::map< std::string, std::map< std::string, std::string > > INIData;

/**
 * @class INIReader.
 * @brief Used to read an INI files.
 *        Helper class that reads INI files and
 *        provides methods to retrieve data from them.
 */
class INIReader
{
public:
    INIReader ();
    INIReader ( const std::string file );
    
    /**
     * @brief Parses the specified file.
     * @param file Filename as a string.
     */
    void parse ( const std::string file );
    
    /**
     * @brief Retrieves an integer.
     * @param section Section where the integer can be found in.
     * @param key INI key of the integer.
     * @param default_value Value to be returned if the specified section does not contain the specified key.
     * @return Integer as a long.
     */
    long GetInteger ( const std::string section, const std::string key, const long default_value );
    /**
     * @brief Retrieves a floating point number.
     * @param section Section where the floating point number can be found in.
     * @param key INI key of the floating point number.
     * @param default_value Value to be returned if the specified section does not contain the specified key.
     * @return Floating point number as a double.
     */
    double GetReal ( const std::string section, const std::string key, const double default_value );
    /**
     * @brief Retrieves a boolean value.
     * @param section Section where the boolean can be found in.
     * @param key INI key of the boolean.
     * @param default_value Value to be returned if the specified section does not contain the specified key.
     * @return Boolean as a bool.
     */
    bool GetBoolean ( const std::string section, const std::string key, const bool default_value );
    /**
     * @brief Retrieves a string.
     * @param section Section where the string can be found in.
     * @param key INI key of the string.
     * @param default_value Value to be returned if the specified section does not contain the specified key.
     * @return String as a std::string.
     */
    std::string GetString ( const std::string section, const std::string key, const std::string default_value );
    /**
     * @brief Retrieves a string.
     * @param section Section where the string can be found in.
     * @param key INI key of the string.
     * @param default_value Value to be returned if the specified section does not contain the specified key.
     * @return String as a std::string.
     */
    std::string Get ( const std::string section, const std::string key, const std::string default_value );
    /**
     * @brief Retrieves a list of sections.
     * @return Sections of the INI file as a std::vector< std::string >.
     */
    std::vector< std::string > Sections ();
    
private:
    // VARS
    INIData mData;
};

#endif // _inireader_h
