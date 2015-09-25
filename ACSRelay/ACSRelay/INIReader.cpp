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
    std::string group, line, l, v;
    std::ifstream f ( file.c_str () );
    
    while ( std::getline ( f, line ) )
    {
        if ( line.find ( "#" ) != std::string::npos )
            line = line.substr ( 0, line.find ( "#" ) );
            
        if ( line.empty () )
            continue;
    
        if ( ( line[ 0 ] == '[' ) && line[ line.length () - 1 ] == ']' )
        {
            group = line.substr ( 1, line.length () - 2 );
        }
        else if ( line.find ( "=" ) != std::string::npos )
        {
            l = line.substr( 0, line.find ( "=" ) );
            l = l.substr ( l.find_first_not_of ( " \t" ), l.find_last_not_of ( " \t" ) + 1 );
            
            v = line.substr ( line.find ( "=" ) );
            v = v.substr ( l.find_first_not_of ( " \t" ) + 1, v.find_last_not_of ( " \t" ) + 1 );
            
            mData[group][l] = v;
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
