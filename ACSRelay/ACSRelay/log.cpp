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

const std::string Log::DEFAULT_LOG_FILE =  "acsrelay.log.txt";
const Log::OutputLevel Log::DEFAULT_LOG_LEVEL = NORMAL_LVL;

Log* Log::mInstance = NULL;

Log::Log () :
    mLogFile(NULL)
{
    Log ( DEFAULT_LOG_LEVEL, DEFAULT_LOG_FILE );
}

Log::Log ( const enum OutputLevel level, const std::string logfile )
{
    mLogFile = NULL;
    mLevel = level;
    mLogFilename = logfile;
    mFileOutputEnabled = true;

    mLogFile = new std::ofstream ( mLogFilename, std::ofstream::app | std::ofstream::out );
    mOutput = &std::cout;

    mTreatWarningsAsErrors = false;

    if ( !mLogFile -> good() || !mLogFile -> is_open() )
    {
        mLogFile -> close();
        mLogFile = NULL;
        mFileOutputEnabled = false;
        *mOutput << "E/ Couldn't open log file!";
    }
}

Log::~Log ()
{
    mLogFile -> close ();
    delete mLogFile;
}

void Log::Start ()
{
    Start ( DEFAULT_LOG_LEVEL, DEFAULT_LOG_FILE );
}

void Log::Start ( const enum OutputLevel level, const std::string logfile )
{
    if ( mInstance == NULL )
    {
        mInstance = new Log ( level, logfile );
    }
}

void Log::Stop ()
{
    delete mInstance;
}

Log& Log::d ()
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    mInstance -> mRequestedLevel = DEBUG_LVL;
    mInstance -> mOutput = &std::cout;

    if ( mInstance -> mRequestedLevel <= mInstance -> mLevel )
    {
        *( mInstance -> mOutput ) << "\nD/ ";

        if ( mInstance -> mFileOutputEnabled )
        {
            *( mInstance -> mLogFile ) << "\n(" << buffer << ") D/ ";
        }
    }

    return *mInstance;
}

Log& Log::e ()
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    mInstance -> mRequestedLevel = ERROR_LVL;
    mInstance -> mOutput = &std::cerr;

    if ( mInstance -> mRequestedLevel <= mInstance -> mLevel )
    {
        *( mInstance -> mOutput ) << "\nE/ ";

        if ( mInstance -> mFileOutputEnabled )
        {
            *( mInstance -> mLogFile ) << "\n(" << buffer << ") E/ ";
        }
    }

    return *mInstance;
}

Log& Log::i ()
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    mInstance -> mRequestedLevel = NORMAL_LVL;
    mInstance -> mOutput = &std::cout;

    if ( mInstance -> mRequestedLevel <= mInstance -> mLevel )
    {
        *( mInstance -> mOutput ) << "\nI/ ";

        if ( mInstance -> mFileOutputEnabled )
        {
            *( mInstance -> mLogFile ) << "\n(" << buffer << ") I/ ";
        }
    }

    return *mInstance;
}

Log& Log::v ()
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    mInstance -> mRequestedLevel = VERBOSE_LVL;
    mInstance -> mOutput = &std::cout;

    if ( mInstance -> mRequestedLevel <= mInstance -> mLevel )
    {
        *( mInstance -> mOutput ) << "\nV/ ";

        if ( mInstance -> mFileOutputEnabled )
        {
            *( mInstance -> mLogFile ) << "\n(" << buffer << ") V/ ";
        }
    }

    return *mInstance;
}

Log& Log::w ()
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    mInstance -> mRequestedLevel = WARNING_LVL;

    if ( mInstance -> mTreatWarningsAsErrors )
        mInstance -> mOutput = &std::cerr;
    else
        mInstance -> mOutput = &std::cout;

    if ( mInstance -> mRequestedLevel <= mInstance -> mLevel )
    {
        *( mInstance -> mOutput ) << "\nW/ ";

        if ( mInstance -> mFileOutputEnabled )
        {
            *( mInstance -> mLogFile ) << "\n(" << buffer << ") W/ ";
        }
    }

    return *mInstance;
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
    mInstance -> mLevel = level;
}

std::string Log::_log_packet ( char* msg, long len )
{
    std::ostringstream r;

    switch ( static_cast<int8_t>(msg[ 0 ]) )
    {
        case ACSProtocol::ACSP_BROADCAST_CHAT:
        {
            r << "ACSP_BROADCAST_CHAT" ;
            int namelen = msg[ 1 ] * 4;
            r << "\n\tMESSAGE: " << std::string ( msg + 2, namelen ) ;
        }; break;
        case ACSProtocol::ACSP_CAR_INFO:
        {
            r << "ACSP_CAR_INFO";
            int index = 1, len;

            r << "\n\tCAR ID: " << int(static_cast<int8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tIS CONNECTED: " << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;

            len = msg[ index ] * 4; index += 1;
            r << "\n\tCAR MODEL: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ] * 4; index += 1;
            r << "\n\tCAR SKIN: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ] * 4; index += 1;
            r << "\n\tDRIVER NAME: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ] * 4; index += 1;
            r << "\n\tDRIVER TEAM: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ] * 4; index += 1;
            r << "\n\tDRIVER GUID: " << std::string ( msg + index, len );
            index += len;

        }; break;
        case ACSProtocol::ACSP_CAR_UPDATE:
        {
            r << "ACSP_CAR_UPDATE";
            int index = 1;

            r << "\n\tCAR ID: " << int(static_cast<int8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tPOSITION: (";

            if ( sizeof ( float ) == 4 )
            {
                r << static_cast<float> ( *( msg + index ) ) << ", ";
                r << static_cast<float> ( *( msg + index + 4 ) ) << ", ";
                r << static_cast<float> ( *( msg + index + 8 ) );
            }
            else if ( sizeof ( double ) == 4 )
            {
                r << static_cast<double> ( *( msg + index ) ) << ", ";
                r << static_cast<double> ( *( msg + index + 4 ) ) << ", ";
                r << static_cast<double> ( *( msg + index + 8 ) );
            }

            r << ")";
            index += 12;

            r << "\n\tVELOCITY: (";

            if ( sizeof ( float ) == 4 )
            {
                r << static_cast<float> ( *( msg + index ) ) << ", ";
                r << static_cast<float> ( *( msg + index + 4 ) ) << ", ";
                r << static_cast<float> ( *( msg + index + 8 ) );
            }
            else if ( sizeof ( double ) == 4 )
            {
                r << static_cast<double> ( *( msg + index ) ) << ", ";
                r << static_cast<double> ( *( msg + index + 4 ) ) << ", ";
                r << static_cast<double> ( *( msg + index + 8 ) );
            }

            r << ")";
            index += 12;

            r << "\n\tGEAR: " << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tENGINE SPEED: " << static_cast<uint16_t> ( *( msg + index ) ); index += 2;

            r << "\n\tNORMALIZED SPLINE POSITION: ";

            if ( sizeof ( float ) == 4 )
                r << static_cast<float> ( *( msg + index ) );
            else if ( sizeof ( double ) == 4 )
                r << static_cast<double> ( *( msg + index ) );

            index += 4;

        }; break;
        case ACSProtocol::ACSP_CHAT:
        {
            r << "ACSP_CHAT";
            int index = 1, len;

            r << "\n\tCAR ID: " << int(static_cast<int8_t> ( *( msg + index ) )); index += 1;

            len = msg[ index ] * 4; index += 1;
            r << "\n\tMESSAGE: " << std::string ( msg + index, len );
            index += len;
        }; break;
        case ACSProtocol::ACSP_CLIENT_EVENT:
        {
            r << "ACSP_CLIENT_EVENT";
            uint8_t evtype = msg[ 1 ];

            int index = 2;

            switch ( evtype )
            {
                case ACSProtocol::ACSP_CE_COLLISION_WITH_CAR:
                {
                    r << "\n\tCOLLISION WITH CAR: " << int(static_cast<int8_t> ( *( msg + index ) ));
                    index += 1;

                } break;
                case ACSProtocol::ACSP_CE_COLLISION_WITH_ENV:
                {
                    r << "\n\tCOLLISION WITH ENVIRONMENT";
                } break;
            }

            r << "\n\tIMPACT SPEED: ";

            if ( sizeof ( float ) == 4 )
                r << static_cast<float> ( *( msg + index ) );
            else if ( sizeof ( double ) == 4 )
                r << static_cast<double> ( *( msg + index ) );

            index += 4;

            r << "\n\tWORLD POSITION: (";

            if ( sizeof ( float ) == 4 )
            {
                r << static_cast<float> ( *( msg + index ) ) << ", ";
                r << static_cast<float> ( *( msg + index + 4 ) ) << ", ";
                r << static_cast<float> ( *( msg + index + 8 ) );
            }
            else if ( sizeof ( double ) == 4 )
            {
                r << static_cast<double> ( *( msg + index ) ) << ", ";
                r << static_cast<double> ( *( msg + index + 4 ) ) << ", ";
                r << static_cast<double> ( *( msg + index + 8 ) );
            }

            r << ")";
            index += 12;

            r << "\n\tRELATIVE POSITION: (";

            if ( sizeof ( float ) == 4 )
            {
                r << static_cast<float> ( *( msg + index ) ) << ", ";
                r << static_cast<float> ( *( msg + index + 4 ) ) << ", ";
                r << static_cast<float> ( *( msg + index + 8 ) );
            }
            else if ( sizeof ( double ) == 4 )
            {
                r << static_cast<double> ( *( msg + index ) ) << ", ";
                r << static_cast<double> ( *( msg + index + 4 ) ) << ", ";
                r << static_cast<double> ( *( msg + index + 8 ) );
            }

            r << ")";
            index += 12;

        }; break;
        case ACSProtocol::ACSP_CLIENT_LOADED:
        {
            r << "ACSP_CLIENT_LOADED";
            r << "\n\tCAR ID: " << unsigned(static_cast<uint8_t> ( msg[ 1 ] ));
        }; break;
        case ACSProtocol::ACSP_CONNECTION_CLOSED:
        {
            r << "ACSP_CONNECTION_CLOSED";
            int len, index = 1;

            len = msg[ index ] * 4; index += 1;
            r << "\n\tDRIVER NAME: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ] * 4; index += 1;
            r << "\n\tDRIVER GUID: " << std::string ( msg + index, len );
            index += len;

            r << "\n\tCAR ID: " << int(static_cast<int8_t> ( *( msg + index ) )); index += 1;

            len = msg[ index ]; index += 1;
            r << "\n\tCAR MODEL: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tCAR SKIN: " << std::string ( msg + index, len );
            index += len;
        }; break;
        case ACSProtocol::ACSP_END_SESSION:
        {
            r << "ACSP_END_SESSION";

            int len = msg[ 1 ] * 4;

            r << "\n\tJSON FILENAME: " << std::string ( msg + 2, len );

        }; break;
        case ACSProtocol::ACSP_ERROR:
        {
            r << "ACSP_ERROR";
            int namelen = msg[ 1 ] * 4;
            r << "\n\tERROR: " << std::string ( msg + 2, namelen ) ;
        }; break;
        case ACSProtocol::ACSP_GET_CAR_INFO:
        {
            r << "ACSP_GET_CAR_INFO";
            r << "\n\tCAR ID: "; r << unsigned(static_cast<uint8_t> ( msg[ 1 ] ));
        }; break;
        case ACSProtocol::ACSP_GET_SESSION_INFO:
        {
            r << "ACSP_GET_SESSION_INFO";
            r << "\n\tSESSION INDEX: " << unsigned(static_cast<uint8_t> ( msg[ 1 ] ));
        }; break;
        case ACSProtocol::ACSP_KICK_USER:
        {
            r << "ACSP_KICK_USER";
            r << "\n\tCAR ID: " << unsigned(static_cast<uint8_t> ( msg[ 1 ] ));
        }; break;
        case ACSProtocol::ACSP_LAP_COMPLETED:
        {
            r << "ACSP_LAP_COMPLETED";
            int index = 1;

            r << "\n\tCAR ID: " << int(static_cast<int8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tLAPTIME: " << static_cast<uint32_t> ( *( msg + index ) ); index += 1;
            r << "\n\tCUTS: " << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;
        }; break;
        case ACSProtocol::ACSP_NEW_CONNECTION:
        {
            r << "ACSP_NEW_CONNECTION";
            int len, index = 1;

            len = msg[ index ] * 4; index += 1;
            r << "\n\tDRIVER NAME: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ] * 4; index += 1;
            r << "\n\tDRIVER GUID: " << std::string ( msg + index, len );
            index += len;

            r << "\n\tCAR ID: " << int(static_cast<int8_t> ( *( msg + index ) )); index += 1;

            len = msg[ index ]; index += 1;
            r << "\n\tCAR MODEL: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tCAR SKIN: " << std::string ( msg + index, len );
            index += len;
        }; break;
        case ACSProtocol::ACSP_NEW_SESSION:
        {
            r << "ACSP_NEW_SESSION";
            int len, index = 1;

            r << "\n\tPROTOCOL VERSION: " << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tSESSION INDEX: "; r << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tCURRENT SESSION INDEX: " << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tSESSION COUNT: " << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;

            len = msg[ 5 ] * 4; index += 1;
            r << "\n\tSERVER NAME: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tTRACK: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tTRACK CONFIG: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tSESSION NAME: " << std::string ( msg + index, len );
            index += len;

            r << "\n\tTYPE: " << unsigned(static_cast<uint8_t> ( *( msg + index ) ));
            index += 1;

            r << "\n\tTIME: " << static_cast<uint16_t> ( *( msg + index ) );
            index += 2;

            r << "\n\tLAPS: " << static_cast<uint16_t> ( *( msg + index ) );
            index += 2;

            r << "\n\tWAIT TIME: " << static_cast<uint16_t> ( *( msg + index ) );
            index += 2;

            r << "\n\tAMBIENT TEMP: " << unsigned(static_cast<uint8_t> ( *( msg + index ) ));
            index += 1;

            r << "\n\tROAD TEMP: " << unsigned(static_cast<uint8_t> ( *( msg + index ) ));
            index += 1;

            len = msg[ index ]; index += 1;
            r << "\n\tWEATHER GRAPHICS: " << std::string ( msg + index, len );
            index += len;

            r << "\n\tELAPSED TIME (ms): " << static_cast<uint32_t> ( *( msg + index ) );
            index += 4;
        }; break;
        case ACSProtocol::ACSP_REALTIMEPOS_INTERVAL:
        {
            r << "ACSP_REALTIMEPOS_INTERVAL";
            r << "\n\tINTERVAL (ms): " << unsigned(static_cast<uint8_t> ( msg[ 1 ] ));
        }; break;
        case ACSProtocol::ACSP_SEND_CHAT:
        {
            r << "ACSP_SEND_CHAT";
            r << "\n\tCAR ID: " << int(static_cast<int8_t> ( msg[ 1 ] ));
            int namelen = msg[ 2 ] * 4;
            r << "\n\tMESSAGE: " << std::string ( msg + 3, namelen ) ;
        }; break;
        case ACSProtocol::ACSP_SESSION_INFO:
        {
            r << "ACSP_SESSION_INFO";
            int len, index = 1;

            r << "\n\tPROTOCOL VERSION: " << int(static_cast<int8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tSESSION INDEX: " << int(static_cast<uint8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tCURRENT SESSION INDEX: " << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tSESSION COUNT: " << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;

            index = 6;
            len = msg[ 5 ] * 4; index += 1;
            r << "\n\tSERVER NAME: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tTRACK: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tTRACK CONFIG: " << std::string ( msg + index, len );
            index += len;

            len = msg[ index ]; index += 1;
            r << "\n\tSESSION NAME: " << std::string ( msg + index, len );
            index += len;

            r << "\n\tTYPE: " << unsigned(static_cast<uint8_t> ( *( msg + index ) ));
            index += 1;

            r << "\n\tTIME: " << static_cast<uint16_t> ( *( msg + index ) );
            index += 2;

            r << "\n\tLAPS: " << static_cast<uint16_t> ( *( msg + index ) );
            index += 2;

            r << "\n\tWAIT TIME: " << static_cast<uint16_t> ( *( msg + index ) );
            index += 2;

            r << "\n\tAMBIENT TEMP: " << unsigned(static_cast<uint8_t> ( *( msg + index ) ));
            index += 1;

            r << "\n\tROAD TEMP: " << unsigned(static_cast<uint8_t> ( *( msg + index ) ));
            index += 1;

            len = msg[ index ]; index += 1;
            r << "\n\tWEATHER GRAPHICS: " << std::string ( msg + index, len );
            index += len;

            r << "\n\tELAPSED TIME (ms): " << static_cast<uint32_t> ( *( msg + index ) );
            index += 4;

        }; break;
        case ACSProtocol::ACSP_SET_SESSION_INFO:
        {
            r << "ACSP_SET_SESSION_INFO";
            int len, index = 1;

            r << "\n\tSESSION INDEX: " << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;
            len = msg[ index ] * 4; index += 1;
            r << "\n\tSESSION NAME: " << std::string ( msg + 3, len ); index += len;
            r << "\n\tSESSION INDEX: " << unsigned(static_cast<uint8_t> ( *( msg + index ) )); index += 1;
            r << "\n\tLAPS: " << static_cast<uint32_t> ( *( msg + index ) ); index += 4;
            r << "\n\tTIME (s): " << static_cast<uint32_t> ( *( msg + index ) ); index += 4;
            r << "\n\tWAIT TIME (s): " << static_cast<int32_t> ( *( msg + index ) ); index += 4;
        }; break;
        case ACSProtocol::ACSP_VERSION:
        {
            r << "ACSP_VERSION";
            r << "\n\tPROTOCOL VERSION: " << int(static_cast<int8_t> ( msg[ 1 ] ));
        }; break;
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
