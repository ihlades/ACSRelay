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

#include "INIReader.h"

#include <algorithm>
#include <fstream>

INIReader::INIReader()
{
}

INIReader::INIReader( const std::string file )
{
    parse( file );
}
void INIReader::parse(const std::string file)
{
    std::string group, line, k, v;
    std::ifstream f ( file.c_str () );
    size_t pos;
    
    while ( std::getline ( f, line ) )
    {

        // Start looking for comments on each line.
        if ( ( pos = line.find_first_of ( "#;" ) ) != std::string::npos )
        {
            while ( pos > 1 && ( line[ pos - 1 ] == '\\' ) )
            {
                if ( pos == line.length() )
                {
                    // We've reached the end of the string.
                    // There is no comment marker, just semicolons
                    // used in the value.
                    pos = line.length();
                    break;
                }
                else
                {
                    // The semicolon we've found is preceded by a
                    // slash, so it's part of a string.
                    // Keep looking for the comment marker.
                    pos = line.find_first_of ( "#;", pos + 1 );
                }
            }
        }

        line = line.substr ( 0, pos );

        // If it's a comment or an empty line we'll ignore it.
        if ( line.empty () )
            continue;

        // Trim right whitespace. This allows to have spaces before
        // comments, without affecting the rest of the line.

        line.erase (
            std::find_if (
                line.rbegin (), line.rend (), std::not1 (
                    std::ptr_fun<int, int> ( std::isspace )
                )
            ).base (),
            line.end ()
        );

        // Check if this line is a group identifier.

        if ( ( line[ 0 ] == '[' ) && line[ line.length () - 1 ] == ']' )
        {
            group = line.substr ( 1, line.length () - 2 );
        }
        else if ( line.find_first_of ( "=:" ) != std::string::npos )
        {
            k = line.substr( 0, line.find_first_of ( "=:" ) );

            // Trim left whitespace for key:
            k.erase (
                k.begin(),
                std::find_if (
                    k.begin (), k.end (), std::not1 (
                        std::ptr_fun<int, int> ( std::isspace )
                    )
                )
            );

            // Trim right whitespace for key:
            k.erase (
                std::find_if (
                    k.rbegin (), k.rend (), std::not1 (
                        std::ptr_fun<int, int> ( std::isspace )
                    )
                ).base (),
                k.end ()
            );
            
            v = line.substr ( line.find_first_of ( "=:" ) + 1 );

            // Trim left whitespace for value:
            v.erase (
                v.begin(),
                std::find_if (
                    v.begin (), v.end (), std::not1 (
                        std::ptr_fun<int, int> ( std::isspace )
                    )
                )
            );

            // Trim right whitespace for value:
            v.erase (
                std::find_if (
                    v.rbegin (), v.rend (), std::not1 (
                        std::ptr_fun<int, int> ( std::isspace )
                    )
                ).base (),
                v.end ()
            );
            
            mData[group][k] = v;
        }
    }
    
    f.close ();
}

std::string INIReader::Get(const std::string section, const std::string key, const std::string default_value)
{
    return GetString ( section, key, default_value );
}

std::string INIReader::GetString(const std::string section, const std::string key, const std::string default_value)
{
    INIData::iterator it;
    
    if ( mData.find ( section ) != mData.end() )
    {
      if ( mData[ section ].find ( key ) != mData[ section ].end() )
      {
          return mData[ section ][ key ];
      }
    }
    
    return default_value;
}

bool INIReader::GetBoolean( const std::string section, const std::string key, const bool default_value )
{
    std::string sVal;
    
    sVal = Get ( section, key, default_value == true ? "TRUE" : "FALSE" );
    std::transform( sVal.begin(), sVal.end(), sVal.begin(), ::tolower );
    
    if ( sVal == "yes" || sVal == "true" || sVal == "on" || sVal == "1" )
        return true;
    else if ( sVal == "no" || sVal == "false" || sVal == "off" || sVal == "0" )
        return false;
    
    return default_value;
}

long INIReader::GetInteger( const std::string section, const std::string key, const long default_value )
{
    std::string sVal;
    long rv;
    
    sVal = Get ( section, key, "" );
    
    if ( sVal == "" )
        return default_value;
        
    const char *st = sVal.c_str ();
    char *end;
    
    rv = strtol ( st, &end, 0 );
    
    if ( end > st )
        return rv;
        
    return default_value;
}

double INIReader::GetReal( const std::string section, const std::string key, const double default_value )
{
    std::string sVal;
    double rv;
    
    sVal = Get ( section, key, "" );
    
    if ( sVal == "" )
        return default_value;
        
    const char *st = sVal.c_str ();
    char *end;
    
    rv = strtod ( st, &end );
    
    if ( end > st )
        return rv;
        
    return default_value;
}

std::vector< std::string > INIReader::Sections ()
{
    std::vector< std::string > rv;
    
    for ( auto it = mData.begin(); it != mData.end(); ++it )
    {
        rv.push_back ( it -> first );
    }
    
    return rv;
}
