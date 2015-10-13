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

#include "log.h"
#include "ACSProtocol.h"

#include <iostream>
#include <sstream>
#include <time.h>
#include <wchar.h>

std::string Log::_log_file =  "acsrelay.log.txt";
Log::OutputLevel Log::_log_level = NORMAL_LVL;

Log::Log ()
    : mOutput(NULL),
      mLogFile(NULL),
#ifdef _DEBUG
      mLevel(DEBUG_LVL),
#else
      mLevel(_log_level),
#endif
      mRequestedLevel(NORMAL_LVL),
      mLogFilename(_log_file),
      mFileOutputEnabled(false),
      mTreatWarningsAsErrors(false)
{
    mOutput = &std::cout;

    if ( mLogFilename != "" )
    {
        mLogFile = new std::ofstream ( mLogFilename, std::ofstream::app | std::ofstream::out );
        mFileOutputEnabled = true;

        if ( !mLogFile -> good() || !mLogFile -> is_open() )
        {
            mLogFile -> close();
            mLogFile = NULL;
            mFileOutputEnabled = false;
            *mOutput << "(E): Couldn't open log file!";
        }
    }

    std::cout << "Log opened.";
    if ( mFileOutputEnabled )
        std::cout << " Writing to \"" << mLogFilename << "\" as well.";
}

Log& Log::d ()
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    GetLogger().mRequestedLevel = DEBUG_LVL;
    GetLogger().mOutput = &std::cout;

    if ( GetLogger().mRequestedLevel <= GetLogger().mLevel )
    {
        *( GetLogger().mOutput ) << "\n(D): " << std::flush;

        if ( GetLogger().mFileOutputEnabled )
        {
            *( GetLogger().mLogFile ) << "\n(" << buffer << ") D/ " << std::flush;
        }
    }

    return GetLogger();
}

Log& Log::e ()
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    GetLogger().mRequestedLevel = ERROR_LVL;
    GetLogger().mOutput = &std::cerr;

    if ( GetLogger().mRequestedLevel <= GetLogger().mLevel )
    {
        *( GetLogger().mOutput ) << "\n(E): ";

        if ( GetLogger().mFileOutputEnabled )
        {
            *( GetLogger().mLogFile ) << "\n(" << buffer << ") E/ ";
        }
    }

    return GetLogger();
}

Log& Log::i ()
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    GetLogger().mRequestedLevel = NORMAL_LVL;
    GetLogger().mOutput = &std::cout;

    if ( GetLogger().mRequestedLevel <= GetLogger().mLevel )
    {
        *( GetLogger().mOutput ) << "\n(I): ";

        if ( GetLogger().mFileOutputEnabled )
        {
            *( GetLogger().mLogFile ) << "\n(" << buffer << ") I/ ";
        }
    }

    return GetLogger();
}

Log& Log::v ()
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    GetLogger().mRequestedLevel = VERBOSE_LVL;
    GetLogger().mOutput = &std::cout;

    if ( GetLogger().mRequestedLevel <= GetLogger().mLevel )
    {
        *( GetLogger().mOutput ) << "\n(V): ";

        if ( GetLogger().mFileOutputEnabled )
        {
            *( GetLogger().mLogFile ) << "\n(" << buffer << ") V/ ";
        }
    }

    return GetLogger();
}

Log& Log::w ()
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    GetLogger().mRequestedLevel = WARNING_LVL;

    if ( GetLogger().mTreatWarningsAsErrors )
        GetLogger().mOutput = &std::cerr;
    else
        GetLogger().mOutput = &std::cout;

    if ( GetLogger().mRequestedLevel <= GetLogger().mLevel )
    {
        *( GetLogger().mOutput ) << "\n(W): ";

        if ( GetLogger().mFileOutputEnabled )
        {
            *( GetLogger().mLogFile ) << "\n(" << buffer << ") W/ ";
        }
    }

    return GetLogger();
}

Log& Log::operator<< ( const char &log )
{
    if ( mRequestedLevel > mLevel )
    {
        return *this;
    }

    (*mOutput) << log << std::flush;

    if ( mFileOutputEnabled )
    {
        *mLogFile << log << std::flush;
    }

    return *this;
}

Log& Log::operator<< ( const std::string &log )
{
    if ( mRequestedLevel > mLevel )
    {
        return *this;
    }

    (*mOutput) << log << std::flush;

    if ( mFileOutputEnabled )
    {
        *mLogFile << log << std::flush;
    }

    return *this;
}

Log& Log::operator<< ( const long &log )
{
    if ( mRequestedLevel > mLevel )
    {
        return *this;
    }

    (*mOutput) << log << std::flush;

    if ( mFileOutputEnabled )
    {
        *mLogFile << log << std::flush;
    }

    return *this;
}

Log& Log::operator<< ( const unsigned long &log )
{
    if ( mRequestedLevel > mLevel )
    {
        return *this;
    }

    (*mOutput) << log << std::flush;

    if ( mFileOutputEnabled )
    {
        *mLogFile << log << std::flush;
    }

    return *this;
}

Log& Log::operator<< ( const double &log )
{
    if ( mRequestedLevel > mLevel )
    {
        return *this;
    }

    (*mOutput) << log << std::flush;

    if ( mFileOutputEnabled )
    {
        *mLogFile << log << std::flush;
    }

    return *this;
}

Log& Log::operator<< ( const bool &log )
{
    if ( mRequestedLevel > mLevel )
    {
        return *this;
    }

    (*mOutput) << ( log ? "TRUE" : "FALSE" ) << std::flush;

    if ( mFileOutputEnabled )
    {
        *mLogFile << ( log ? "TRUE" : "FALSE" ) << std::flush;
    }

    return *this;
}

// Explicit instantiation:
template Log& Log::operator<<<char*, long> ( const _log_manip<char*, long> &manip );

template<class T1, class T2>
Log& Log::operator << ( const _log_manip<T1, T2> &manip )
{
    if ( mRequestedLevel > mLevel )
    {
        return *this;
    }

    std::string out = manip._call ();

    (*mOutput) << out << std::flush;

    if ( mFileOutputEnabled )
    {
        *mLogFile << out << std::flush;
    }

    return *this;
}

void Log::SetOutputLevel ( const enum OutputLevel level )
{
    GetLogger().mLevel = level;
}

void Log::SetOutputFile ( const std::string fn )
{
    _log_file = fn;
}

std::string Log::_log_packet ( char* msg, long len )
{
    std::ostringstream r;

    switch ( *(reinterpret_cast<int8_t*> ( msg )) )
    {
        case ACSProtocol::ACSP_BROADCAST_CHAT:
        {
            r << "\n\t+-------------------+";
            r << "\n\t ACSP_BROADCAST_CHAT ";
            r << "\n\t+-------------------+";

            int namelen = msg[ 1 ];

            std::u32string utf32_string ( reinterpret_cast<char32_t*>(msg + 2), namelen );
            convert32 converter;
            std::string ws = converter.to_bytes(utf32_string);
            
            r << L"\n\tMESSAGE: \"" << ws << "\"";
        }; break;
        case ACSProtocol::ACSP_CAR_INFO:
        {
            r << "\n\t+-------------+";
            r << "\n\t ACSP_CAR_INFO ";
            r << "\n\t+-------------+";

            int index = 1, len;
            std::u32string utf32_string;
            convert32 converter;
            std::string ws;

            r << "\n\tCAR ID: " << int( *(reinterpret_cast<uint8_t*> ( msg + index )) ); index += 1;
            r << "\n\tIS CONNECTED: " << unsigned( *(reinterpret_cast<uint8_t*> ( msg + index )) ); index += 1;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tCAR MODEL: " << ws;
            index += len * 4;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tCAR SKIN: " << ws;
            index += len * 4;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tDRIVER NAME: " << ws;
            index += len * 4;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tDRIVER TEAM: " << ws;
            index += len * 4;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tDRIVER GUID: " << ws;
            index += len * 4;

        }; break;
        case ACSProtocol::ACSP_CAR_UPDATE:
        {
            r << "\n\t+---------------+";
            r << "\n\t ACSP_CAR_UPDATE ";
            r << "\n\t+---------------+";

            int index = 1;

            r << "\n\tCAR ID: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;
            r << "\n\tPOSITION: (";

            if ( sizeof ( float ) == 4 )
            {
                r << *(reinterpret_cast<float*> ( msg + index ) ) << ", ";
                r << *(reinterpret_cast<float*> ( msg + index + 4 ) ) << ", ";
                r << *(reinterpret_cast<float*> ( msg + index + 8 ) );
            }
            else if ( sizeof ( double ) == 4 )
            {
                r << *(reinterpret_cast<double*> ( msg + index ) ) << ", ";
                r << *(reinterpret_cast<double*> ( msg + index + 4 ) ) << ", ";
                r << *(reinterpret_cast<double*> ( msg + index + 8 ) );
            }

            r << ")";
            index += 12;

            r << "\n\tVELOCITY: (";

            if ( sizeof ( float ) == 4 )
            {
                r << *(reinterpret_cast<float*> ( msg + index ) ) << ", ";
                r << *(reinterpret_cast<float*> ( msg + index + 4 ) ) << ", ";
                r << *(reinterpret_cast<float*> ( msg + index + 8 ) );
            }
            else if ( sizeof ( double ) == 4 )
            {
                r << *(reinterpret_cast<double*> ( msg + index ) ) << ", ";
                r << *(reinterpret_cast<double*> ( msg + index + 4 ) ) << ", ";
                r << *(reinterpret_cast<double*> ( msg + index + 8 ) );
            }

            r << ")";
            index += 12;

            r << "\n\tGEAR: " << unsigned( *(reinterpret_cast<uint8_t*> ( msg + index )) ); index += 1;
            r << "\n\tENGINE SPEED: " << *(reinterpret_cast<uint16_t*> ( msg + index )); index += 2;

            r << "\n\tNORMALIZED SPLINE POSITION: ";

            if ( sizeof ( float ) == 4 )
                r << *(reinterpret_cast<float*> ( msg + index ) );
            else if ( sizeof ( double ) == 4 )
                r << *(reinterpret_cast<double*> ( msg + index ) );

            index += 4;

        }; break;
        case ACSProtocol::ACSP_CHAT:
        {
            r << "\n\t+---------+";
            r << "\n\t ACSP_CHAT ";
            r << "\n\t+---------+";

            int index = 1, len;
            std::u32string utf32_string;
            convert32 converter;
            std::string ws;

            r << "\n\tCAR ID: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tMESSAGE: \"" << ws << "\"";
            index += len * 4;
        }; break;
        case ACSProtocol::ACSP_CLIENT_EVENT:
        {
            r << "\n\t+-----------------+";
            r << "\n\t ACSP_CLIENT_EVENT ";
            r << "\n\t+-----------------+";

            uint8_t evtype = msg[ 1 ];

            int index = 2;

            switch ( evtype )
            {
                case ACSProtocol::ACSP_CE_COLLISION_WITH_CAR:
                {
                    r << "\n\tCOLLISION WITH CAR: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) );
                    index += 1;

                } break;
                case ACSProtocol::ACSP_CE_COLLISION_WITH_ENV:
                {
                    r << "\n\tCOLLISION WITH ENVIRONMENT";
                } break;
            }

            r << "\n\tIMPACT SPEED: ";

            if ( sizeof ( float ) == 4 )
                r << *(reinterpret_cast<float*> ( msg + index ));
            else if ( sizeof ( double ) == 4 )
                r << *(reinterpret_cast<double*> ( msg + index ));

            index += 4;

            r << "\n\tWORLD POSITION: (";

            if ( sizeof ( float ) == 4 )
            {
                r << *(reinterpret_cast<float*> ( msg + index )) << ", ";
                r << *(reinterpret_cast<float*> ( msg + index + 4 )) << ", ";
                r << *(reinterpret_cast<float*> ( msg + index + 8 ));
            }
            else if ( sizeof ( double ) == 4 )
            {
                r << *(reinterpret_cast<double*> ( msg + index )) << ", ";
                r << *(reinterpret_cast<double*> ( msg + index + 4 )) << ", ";
                r << *(reinterpret_cast<double*> ( msg + index + 8 ));
            }

            r << ")";
            index += 12;

            r << "\n\tRELATIVE POSITION: (";

            if ( sizeof ( float ) == 4 )
            {
                r << *(reinterpret_cast<float*> ( msg + index )) << ", ";
                r << *(reinterpret_cast<float*> ( msg + index + 4 )) << ", ";
                r << *(reinterpret_cast<float*> ( msg + index + 8 ));
            }
            else if ( sizeof ( double ) == 4 )
            {
                r << *(reinterpret_cast<double*> ( msg + index )) << ", ";
                r << *(reinterpret_cast<double*> ( msg + index + 4 )) << ", ";
                r << *(reinterpret_cast<double*> ( msg + index + 8 ));
            }

            r << ")";
            index += 12;

        }; break;
        case ACSProtocol::ACSP_CLIENT_LOADED:
        {
            r << "\n\t+------------------+";
            r << "\n\t ACSP_CLIENT_LOADED ";
            r << "\n\t+------------------+";

            r << "\n\tCAR ID: " << unsigned( *(reinterpret_cast<int8_t*> ( msg + 1 )) );
        }; break;
        case ACSProtocol::ACSP_CONNECTION_CLOSED:
        {
            r << "\n\t+----------------------+";
            r << "\n\t ACSP_CONNECTION_CLOSED ";
            r << "\n\t+----------------------+";

            int len, index = 1;
            std::u32string utf32_string;
            convert32 converter;
            std::string ws;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tDRIVER NAME: " << ws;
            index += len * 4;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tDRIVER GUID: " << ws;
            index += len * 4;

            r << "\n\tCAR ID: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;

            len = msg[ index ]; index += 1;
            r << "\n\tCAR MODEL: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tCAR SKIN: " << std::string ( msg + index, len );
            index += len;
        }; break;
        case ACSProtocol::ACSP_END_SESSION:
        {
            r << "\n\t+----------------+";
            r << "\n\t ACSP_END_SESSION ";
            r << "\n\t+----------------+";

            int len = msg[ 1 ];
            std::u32string utf32_string;
            convert32 converter;
            std::string ws;

            len = msg[ 2 ];
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + 2), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tJSON FILENAME: " << ws;

        }; break;
        case ACSProtocol::ACSP_ERROR:
        {
            r << "\n\t+----------+";
            r << "\n\t ACSP_ERROR ";
            r << "\n\t+----------+";

            int namelen = msg[ 1 ] * 4;
            r << "\n\tERROR: " << std::string ( msg + 2, namelen ) ;
        }; break;
        case ACSProtocol::ACSP_GET_CAR_INFO:
        {
            r << "\n\t+-----------------+";
            r << "\n\t ACSP_GET_CAR_INFO ";
            r << "\n\t+-----------------+";

            r << "\n\tCAR ID: "; r << int( *(reinterpret_cast<int8_t*> ( msg + 1 )) );
        }; break;
        case ACSProtocol::ACSP_GET_SESSION_INFO:
        {
            r << "\n\t+---------------------+";
            r << "\n\t ACSP_GET_SESSION_INFO ";
            r << "\n\t+---------------------+";

            r << "\n\tSESSION INDEX: " <<  *(reinterpret_cast<int16_t*> ( msg + 1 ));
        }; break;
        case ACSProtocol::ACSP_KICK_USER:
        {
            r << "\n\t+--------------+";
            r << "\n\t ACSP_KICK_USER ";
            r << "\n\t+--------------+";

            r << "\n\tCAR ID: " << int( *(reinterpret_cast<int8_t*> ( msg + 1 )) );
        }; break;
        case ACSProtocol::ACSP_LAP_COMPLETED:
        {
            r << "\n\t+------------------+";
            r << "\n\t ACSP_LAP_COMPLETED ";
            r << "\n\t+------------------+";

            int index = 1;

            r << "\n\tCAR ID: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;
            r << "\n\tLAPTIME: " << *(reinterpret_cast<uint32_t*> ( msg + index ) ); index += 1;
            r << "\n\tCUTS: " << int( *(reinterpret_cast<uint16_t*> ( msg + index )) ); index += 1;
        }; break;
        case ACSProtocol::ACSP_NEW_CONNECTION:
        {
            r << "\n\t+-------------------+";
            r << "\n\t ACSP_NEW_CONNECTION ";
            r << "\n\t+-------------------+";

            int len, index = 1;
            std::u32string utf32_string;
            convert32 converter;
            std::string ws;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tDRIVER NAME: " << ws;
            index += len * 4;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tDRIVER GUID: " << ws;
            index += len * 4;

            r << "\n\tCAR ID: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;

            len = msg[ index ]; index += 1;
            r << "\n\tCAR MODEL: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tCAR SKIN: " << std::string ( msg + index, len );
            index += len;
        }; break;
        case ACSProtocol::ACSP_NEW_SESSION:
        {
            r << "\n\t+----------------+";
            r << "\n\t ACSP_NEW_SESSION ";
            r << "\n\t+----------------+";
            int len, index = 1;
            std::u32string utf32_string;
            convert32 converter;
            std::string ws;

            r << "\n\tPROTOCOL VERSION: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;
            r << "\n\tSESSION INDEX: "; r << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;
            r << "\n\tCURRENT SESSION INDEX: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;
            r << "\n\tSESSION COUNT: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tSERVER NAME: " << ws;
            index += len * 4;

            len = msg[ index ]; index += 1;
            r << "\n\tTRACK: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tTRACK CONFIG: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tSESSION NAME: " << std::string ( msg + index, len );
            index += len;

            r << "\n\tTYPE: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) );
            index += 1;

            r << "\n\tTIME: " << *(reinterpret_cast<uint16_t*> ( msg + index ) );
            index += 2;

            r << "\n\tLAPS: " << *(reinterpret_cast<uint16_t*> ( msg + index ) );
            index += 2;

            r << "\n\tWAIT TIME: " << *(reinterpret_cast<uint16_t*> ( msg + index ) );
            index += 2;

            r << "\n\tAMBIENT TEMP: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) );
            index += 1;

            r << "\n\tROAD TEMP: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) );
            index += 1;

            len = msg[ index ]; index += 1;
            r << "\n\tWEATHER GRAPHICS: " << std::string ( msg + index, len );
            index += len;

            r << "\n\tELAPSED TIME (ms): " << *(reinterpret_cast<uint32_t*> ( msg + index ) );
            index += 4;
        }; break;
        case ACSProtocol::ACSP_REALTIMEPOS_INTERVAL:
        {
            r << "\n\t+-------------------------+";
            r << "\n\t ACSP_REALTIMEPOS_INTERVAL ";
            r << "\n\t+-------------------------+";

            // In the sample UDP plugin, provided by Kunos, the INTERVAL is a UINT16,
            // but in practice it's an UINT8. So let's handle both, just in case...
            if ( len > 2 )
            {
                r << "\n\tINTERVAL (ms): " << int( *(reinterpret_cast<uint16_t*> ( msg + 1 )) );
            }
            else if ( len == 2 )
            {
                r << "\n\tINTERVAL (ms): " << int( *(reinterpret_cast<uint8_t*> ( msg + 1 )) );
            }
        }; break;
        case ACSProtocol::ACSP_SEND_CHAT:
        {
            r << "\n\t+--------------+";
            r << "\n\t ACSP_SEND_CHAT ";
            r << "\n\t+--------------+";

            r << "\n\tCAR ID: " << int( *(reinterpret_cast<int8_t*> ( msg + 1 )) );
            int namelen = msg[ 2 ];

            std::u32string utf32_string ( reinterpret_cast<char32_t*>(msg + 3), namelen );
            convert32 converter;
            std::string ws = converter.to_bytes(utf32_string);

            r << L"\n\tMESSAGE: \"" << ws << "\"";
        }; break;
        case ACSProtocol::ACSP_SESSION_INFO:
        {
            r << "\n\t+-----------------+";
            r << "\n\t ACSP_SESSION_INFO ";
            r << "\n\t+-----------------+";

            int len, index = 1;
            std::u32string utf32_string;
            convert32 converter;
            std::string ws;

            r << "\n\tPROTOCOL VERSION: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;
            r << "\n\tSESSION INDEX: "; r << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;
            r << "\n\tCURRENT SESSION INDEX: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;
            r << "\n\tSESSION COUNT: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tSERVER NAME: " << ws;
            index += len * 4;

            len = msg[ index ]; index += 1;
            r << "\n\tTRACK: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tTRACK CONFIG: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tSESSION NAME: " << std::string ( msg + index, len );
            index += len;

            r << "\n\tTYPE: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) );
            index += 1;

            r << "\n\tTIME: " << *(reinterpret_cast<uint16_t*> ( msg + index ) );
            index += 2;

            r << "\n\tLAPS: " << *(reinterpret_cast<uint16_t*> ( msg + index ) );
            index += 2;

            r << "\n\tWAIT TIME: " << *(reinterpret_cast<uint16_t*> ( msg + index ) );
            index += 2;

            r << "\n\tAMBIENT TEMP: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) );
            index += 1;

            r << "\n\tROAD TEMP: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) );
            index += 1;

            len = msg[ index ]; index += 1;
            r << "\n\tWEATHER GRAPHICS: " << std::string ( msg + index, len );
            index += len;

            r << "\n\tELAPSED TIME (ms): " << *(reinterpret_cast<uint32_t*> ( msg + index ) );
            index += 4;

        }; break;
        case ACSProtocol::ACSP_SET_SESSION_INFO:
        {
            r << "\n\t+---------------------+";
            r << "\n\t ACSP_SET_SESSION_INFO ";
            r << "\n\t+---------------------+";

            int len, index = 1;
            std::u32string utf32_string;
            convert32 converter;
            std::string ws;

            r << "\n\tSESSION INDEX: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;

            len = msg[ index ]; index += 1;
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + index), len );
            ws = converter.to_bytes(utf32_string);
            r << "\n\tSESSION NAME: " << ws; index += len * 4;

            r << "\n\tSESSION TYPE: " << int( *(reinterpret_cast<int8_t*> ( msg + index )) ); index += 1;
            r << "\n\tLAPS: " << *(reinterpret_cast<uint32_t*> ( msg + index ) ); index += 4;
            r << "\n\tTIME (s): " << *(reinterpret_cast<uint32_t*> ( msg + index ) ); index += 4;
            r << "\n\tWAIT TIME (s): " << *(reinterpret_cast<int32_t*> ( msg + index ) ); index += 4;
        }; break;
        case ACSProtocol::ACSP_NEXT_SESSION:
        {
            r << "\n\t+-----------------+";
            r << "\n\t ACSP_NEXT_SESSION ";
            r << "\n\t+-----------------+";
        } break;
        case ACSProtocol::ACSP_RESTART_SESSION:
        {
            r << "\n\t+--------------------+";
            r << "\n\t ACSP_RESTART_SESSION ";
            r << "\n\t+--------------------+";
        } break;
        case ACSProtocol::ACSP_ADMIN_COMMAND:
        {
            r << "\n\t+------------------+";
            r << "\n\t ACSP_ADMIN_COMMAND ";
            r << "\n\t+------------------+";

            int len;
            std::u32string utf32_string;
            convert32 converter;
            std::string ws;

            len = msg[ 1 ];
            utf32_string = std::u32string( reinterpret_cast<char32_t*>(msg + 2), len );
            ws = converter.to_bytes(utf32_string);

            r << "\n\tCOMMAND: "<< ws;
        } break;
        case ACSProtocol::ACSP_VERSION:
        {
            r << "\n\t+------------+";
            r << "\n\t ACSP_VERSION ";
            r << "\n\t+------------+";

            r << "\n\tPROTOCOL VERSION: " << int( *(reinterpret_cast<int8_t*> ( msg + 1 )) );
        }; break;
        default:
        {
            r << "\n\tUnknown or corrupt packet";
        } break;
    }

    return r.str ();
}

Log::_log_manip<char*, long> Log::Packet ( char* msg, long len )
{
    _log_manip<char*, long> manip ( Log::_log_packet, msg, len );

    return manip;
}

template<class T1, class T2>
Log::_log_manip<T1, T2>::_log_manip (std::string (*fp) (T1 arg1, T2 arg2),T1 arg1, T2 arg2)
{
    pf = fp;
    _arg1 = arg1;
    _arg2 = arg2;
}
