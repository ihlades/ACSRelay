#include "log.h"
#include "ACSProtocol.h"

#include <iostream>
#include <time.h>

const std::string Log::DEFAULT_LOG_FILE =  "acsrelay.log.txt";
const Log::OutputLevel Log::DEFAULT_LOG_LEVEL = Log::OutputLevel::NORMAL;

Log* Log::mInstance = nullptr;

Log::Log ()
{
    Log ( DEFAULT_LOG_LEVEL, DEFAULT_LOG_FILE );
}

Log::Log ( const enum OutputLevel level, const std::string logfile )
{
    mLogFile = nullptr;
    mLevel = level;
    mLogFilename = logfile;
    mFileOutputEnabled = true;
    
    mLogFile = new std::ofstream ( mLogFilename, std::ofstream::app | std::ofstream::out );
    mOutput = &std::cout;
    
    mTreatWarningsAsErrors = false;
    
    if ( !mLogFile -> good() || !mLogFile -> is_open() )
    {
        mLogFile -> close();
        mLogFile = nullptr;
        mFileOutputEnabled = false;
        *mOutput << "E/ Couldn't open log file!";
    }
}

Log::~Log ()
{
    mLogFile -> close ();
}

void Log::Start ()
{
    Start ( DEFAULT_LOG_LEVEL, DEFAULT_LOG_FILE );
}

void Log::Start ( const enum OutputLevel level, const std::string logfile )
{
    if ( mInstance == nullptr )
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
    
    mInstance -> mRequestedLevel = OutputLevel::DEBUG;
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
    
    mInstance -> mRequestedLevel = OutputLevel::ERROR;
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
    
    mInstance -> mRequestedLevel = OutputLevel::NORMAL;
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
    
    mInstance -> mRequestedLevel = OutputLevel::VERBOSE;
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
    
    mInstance -> mRequestedLevel = OutputLevel::WARNING;
    
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
    
    std::cout << log << std::flush;
    
    if ( mFileOutputEnabled )
    {
        *mLogFile << log << std::flush;
    }
    
    return *this;
}

Log& Log::operator<< ( const long long &log )
{
    if ( mRequestedLevel > mLevel )
    {
        return *this;
    }
    
    std::cout << log << std::flush;
    
    if ( mFileOutputEnabled )
    {
        *mLogFile << log << std::flush;
    }
    
    return *this;
}

Log& Log::operator<< ( const unsigned long long &log )
{
    if ( mRequestedLevel > mLevel )
    {
        return *this;
    }
    
    std::cout << log << std::flush;
    
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
    
    std::cout << log << std::flush;
    
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
    
    std::cout << ( log ? "TRUE" : "FALSE" ) << std::flush;
    
    if ( mFileOutputEnabled )
    {
        *mLogFile << ( log ? "TRUE" : "FALSE" ) << std::flush;
    }
    
    return *this;
}

void Log::setOutputLevel ( const enum OutputLevel level )
{
    mInstance -> mLevel = level;
}

// TODO: Output actual packet data as well!

std::string LogPacket::ToString () const
{
    std::string r;
    switch ( (int8_t) mMsg[ 0 ] )
    {
        case ACSProtocol::ACSP_BROADCAST_CHAT:
        {
            r = "ACSP_BROADCAST_CHAT" ;
            int namelen = mMsg[ 1 ] * 4;
            r += "\n\tMESSAGE: "; r += std::string ( mMsg + 2, namelen ) ;
        }; break;
        case ACSProtocol::ACSP_CAR_INFO:
        {
            r = "ACSP_CAR_INFO";
            int index = 1, len;
            
            r += "\n\tCAR ID: "; r += ( int8_t ) *( mMsg + index ); index += 1;
            r += "\n\tIS CONNECTED: "; r += ( uint8_t ) *( mMsg + index ); index += 1;
            
            len = mMsg[ index ] * 4;
            r += "\n\tCAR MODEL: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ] * 4;
            r += "\n\tCAR SKIN: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ] * 4;
            r += "\n\tDRIVER NAME: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ] * 4;
            r += "\n\tDRIVER TEAM: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ] * 4;
            r += "\n\tDRIVER GUID: "; r += std::string ( mMsg + index, len );
            index += len;
            
        }; break;
        case ACSProtocol::ACSP_CAR_UPDATE:
        {
            r = "ACSP_CAR_UPDATE";
            int index = 1;
            
            r += "\n\tCAR ID: "; r += ( int8_t ) *( mMsg + index ); index += 1;
            r += "\n\tPOSITION: (";
            
            if ( sizeof ( float ) == 4 )
            {
                r += ( float ) *( mMsg + index );
                r += ", ";
                r += ( float ) *( mMsg + index + 4 );
                r += ", ";
                r += ( float ) *( mMsg + index + 8 );
            }
            else if ( sizeof ( double ) == 4 )
            {
                r += ( double ) *( mMsg + index );
                r += ", ";
                r += ( double ) *( mMsg + index + 4 );
                r += ", ";
                r += ( double ) *( mMsg + index + 8 );
            }
            
            r += ")";
            index += 12;
            
            r += "\n\tVELOCITY: (";
            
            if ( sizeof ( float ) == 4 )
            {
                r += ( float ) *( mMsg + index );
                r += ", ";
                r += ( float ) *( mMsg + index + 4 );
                r += ", ";
                r += ( float ) *( mMsg + index + 8 );
            }
            else if ( sizeof ( double ) == 4 )
            {
                r += ( double ) *( mMsg + index );
                r += ", ";
                r += ( double ) *( mMsg + index + 4 );
                r += ", ";
                r += ( double ) *( mMsg + index + 8 );
            }
            
            r += ")";
            index += 12;
            
            r += "\n\tGEAR: "; r += ( uint8_t ) *( mMsg + index ); index += 1;
            r += "\n\tENGINE SPEED: "; r += ( uint16_t ) *( mMsg + index ); index += 2;
            
            r += "\n\tNORMALIZED SPLINE POSITION: ";
            
            if ( sizeof ( float ) == 4 )
                r += ( float ) *( mMsg + index );
            else if ( sizeof ( double ) == 4 )
                r += ( double ) *( mMsg + index );
            
            index += 4;
            
        }; break;
        case ACSProtocol::ACSP_CHAT:
        {
            r = "ACSP_CHAT";
            int index = 1, len;
            
            r += "\n\tCAR ID: "; r += ( int8_t ) *( mMsg + index ); index += 1;
            
            len = mMsg[ index ] * 4;
            r += "\n\tMESSAGE: "; r += std::string ( mMsg + index, len );
            index += len;
        }; break;
        case ACSProtocol::ACSP_CLIENT_EVENT:
        {
            r = "ACSP_CLIENT_EVENT";
            uint8_t evtype = mMsg[ 1 ];
            
            int index = 2;
            
            switch ( evtype )
            {
                case ACSProtocol::ACSP_CE_COLLISION_WITH_CAR:
                {
                    r += "\n\tCOLLISION WITH CAR: "; r += ( int8_t ) *( mMsg + index );
                    index += 1;
                    
                } break;
                case ACSProtocol::ACSP_CE_COLLISION_WITH_ENV:
                {
                    r += "\n\tCOLLISION WITH ENVIRONMENT";
                } break;
            }
            
            r += "\n\tIMPACT SPEED: ";
            
            if ( sizeof ( float ) == 4 )
                r += ( float ) *( mMsg + index );
            else if ( sizeof ( double ) == 4 )
                r += ( double ) *( mMsg + index );
            
            index += 4;
            
            r += "\n\tWORLD POSITION: (";
            
            if ( sizeof ( float ) == 4 )
            {
                r += ( float ) *( mMsg + index );
                r += ", ";
                r += ( float ) *( mMsg + index + 4 );
                r += ", ";
                r += ( float ) *( mMsg + index + 8 );
            }
            else if ( sizeof ( double ) == 4 )
            {
                r += ( double ) *( mMsg + index );
                r += ", ";
                r += ( double ) *( mMsg + index + 4 );
                r += ", ";
                r += ( double ) *( mMsg + index + 8 );
            }
            
            r += ")";
            index += 12;
            
            r += "\n\tRELATIVE POSITION: (";
            
            if ( sizeof ( float ) == 4 )
            {
                r += ( float ) *( mMsg + index );
                r += ", ";
                r += ( float ) *( mMsg + index + 4 );
                r += ", ";
                r += ( float ) *( mMsg + index + 8 );
            }
            else if ( sizeof ( double ) == 4 )
            {
                r += ( double ) *( mMsg + index );
                r += ", ";
                r += ( double ) *( mMsg + index + 4 );
                r += ", ";
                r += ( double ) *( mMsg + index + 8 );
            }
            
            r += ")";
            index += 12;
            
        }; break;
        case ACSProtocol::ACSP_CLIENT_LOADED:
        {
            r = "ACSP_CLIENT_LOADED";
            r += "\n\tCAR ID: "; r += ( uint8_t ) mMsg[ 1 ];
        }; break;
        case ACSProtocol::ACSP_CONNECTION_CLOSED:
        {
            r = "ACSP_CONNECTION_CLOSED";
            int len, index = 1;
            
            len = mMsg[ index ] * 4;
            r += "\n\tDRIVER NAME: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ] * 4;
            r += "\n\tDRIVER GUID: "; r += std::string ( mMsg + index, len );
            index += len;
            
            r += "\n\tCAR ID: "; r += ( int8_t ) *( mMsg + index ); index += 1;
            
            len = mMsg[ index ] * 4;
            r += "\n\tCAR MODEL: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ] * 4;
            r += "\n\tCAR SKIN: "; r += std::string ( mMsg + index, len );
            index += len;
        }; break;
        case ACSProtocol::ACSP_END_SESSION:
        {
            r = "ACSP_END_SESSION";
            
            int len = mMsg[ 1 ] * 4;
            
            r += "\n\tJSON FILENAME: "; r += std::string ( mMsg + 2, len );
            
        }; break;
        case ACSProtocol::ACSP_ERROR:
        {
            r = "ACSP_ERROR";
            int namelen = mMsg[ 1 ] * 4;
            r += "\n\tERROR: "; r += std::string ( mMsg + 2, namelen ) ;
        }; break;
        case ACSProtocol::ACSP_GET_CAR_INFO:
        {
            r = "ACSP_GET_CAR_INFO";
            r += "\n\tCAR ID: "; r += ( int8_t ) mMsg[ 1 ];
        }; break;
        case ACSProtocol::ACSP_GET_SESSION_INFO:
        {
            r = "ACSP_GET_SESSION_INFO";
            r += "\n\tSESSION INDEX: "; r += ( int16_t ) ( *( mMsg + 1 ) );
        }; break;
        case ACSProtocol::ACSP_KICK_USER:
        {
            r = "ACSP_KICK_USER";
            r += "\n\tCAR ID: "; r += ( uint8_t ) mMsg[ 1 ];
        }; break;
        case ACSProtocol::ACSP_LAP_COMPLETED:
        {
            r = "ACSP_LAP_COMPLETED";
            int index = 1;
            
            r += "\n\tCAR ID: "; r += ( int8_t ) *( mMsg + index ); index += 1;
            r += "\n\tLAPTIME: "; r += ( uint32_t ) *( mMsg + index ); index += 1;
            r += "\n\tCUTS: "; r += ( uint8_t ) *( mMsg + index ); index += 1;
        }; break;
        case ACSProtocol::ACSP_NEW_CONNECTION:
        {
            r = "ACSP_NEW_CONNECTION";
            int len, index = 1;
            
            len = mMsg[ index ] * 4;
            r += "\n\tDRIVER NAME: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ] * 4;
            r += "\n\tDRIVER GUID: "; r += std::string ( mMsg + index, len );
            index += len;
            
            r += "\n\tCAR ID: "; r += ( int8_t ) *( mMsg + index ); index += 1;
            
            len = mMsg[ index ] * 4;
            r += "\n\tCAR MODEL: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ] * 4;
            r += "\n\tCAR SKIN: "; r += std::string ( mMsg + index, len );
            index += len;
        }; break;
        case ACSProtocol::ACSP_NEW_SESSION:
        {
            r = "ACSP_NEW_SESSION";
            int len, index;
            
            r += "\n\tPROTOCOL VERSION: "; r += ( int8_t ) mMsg[ 1 ];
            r += "\n\tSESSION INDEX: "; r += ( uint8_t ) mMsg[ 2 ];
            r += "\n\tCURRENT SESSION INDEX: "; r += ( uint8_t ) mMsg[ 3 ];
            r += "\n\tSESSION COUNT: "; r += ( uint8_t ) mMsg[ 4 ];
            
            index = 6;
            len = mMsg[ 5 ] * 4;
            r += "\n\tSERVER NAME: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ];
            r += "\n\tTRACK: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ];
            r += "\n\tTRACK CONFIG: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ];
            r += "\n\tSESSION NAME: "; r += std::string ( mMsg + index, len );
            index += len;
            
            r += "\n\tTYPE: "; r += ( uint8_t ) *( mMsg + index );
            index += 1;
            
            r += "\n\tTIME: "; r += ( uint16_t ) *( mMsg + index );
            index += 2;
            
            r += "\n\tLAPS: "; r += ( uint16_t ) *( mMsg + index );
            index += 2;
            
            r += "\n\tWAIT TIME: "; r += ( uint16_t ) *( mMsg + index );
            index += 2;
            
            r += "\n\tAMBIENT TEMP: "; r += ( uint8_t ) *( mMsg + index );
            index += 1;
            
            r += "\n\tROAD TEMP: "; r += ( uint8_t ) *( mMsg + index );
            index += 1;
            
            len = mMsg[ index ];
            r += "\n\tWEATHER GRAPHICS: "; r += std::string ( mMsg + index, len );
            index += len;
            
            r += "\n\tELAPSED TIME (ms): "; r += ( uint32_t ) *( mMsg + index );
            index += 2;
        }; break;
        case ACSProtocol::ACSP_REALTIMEPOS_INTERVAL:
        {
            r = "ACSP_REALTIMEPOS_INTERVAL";
            r += "\n\tINTERVAL (ms): "; r += ( uint16_t ) ( *( mMsg + 1 ) );
        }; break;
        case ACSProtocol::ACSP_SEND_CHAT:
        {
            r = "ACSP_SEND_CHAT";
            r += "\n\tCAR ID: "; r += ( int8_t ) mMsg[ 1 ];
            int namelen = mMsg[ 2 ] * 4;
            r += "\n\tMESSAGE: "; r += std::string ( mMsg + 3, namelen ) ;
        }; break;
        case ACSProtocol::ACSP_SESSION_INFO:
        {
            r = "ACSP_SESSION_INFO";
            int len, index;
            
            r += "\n\tPROTOCOL VERSION: "; r += ( int8_t ) mMsg[ 1 ];
            r += "\n\tSESSION INDEX: "; r += ( uint8_t ) mMsg[ 2 ];
            r += "\n\tCURRENT SESSION INDEX: "; r += ( uint8_t ) mMsg[ 3 ];
            r += "\n\tSESSION COUNT: "; r += ( uint8_t ) mMsg[ 4 ];
            
            index = 6;
            len = mMsg[ 5 ] * 4;
            r += "\n\tSERVER NAME: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ];
            r += "\n\tTRACK: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ];
            r += "\n\tTRACK CONFIG: "; r += std::string ( mMsg + index, len );
            index += len;
            
            len = mMsg[ index ];
            r += "\n\tSESSION NAME: "; r += std::string ( mMsg + index, len );
            index += len;
            
            r += "\n\tTYPE: "; r += ( uint8_t ) *( mMsg + index );
            index += 1;
            
            r += "\n\tTIME: "; r += ( uint16_t ) *( mMsg + index );
            index += 2;
            
            r += "\n\tLAPS: "; r += ( uint16_t ) *( mMsg + index );
            index += 2;
            
            r += "\n\tWAIT TIME: "; r += ( uint16_t ) *( mMsg + index );
            index += 2;
            
            r += "\n\tAMBIENT TEMP: "; r += ( uint8_t ) *( mMsg + index );
            index += 1;
            
            r += "\n\tROAD TEMP: "; r += ( uint8_t ) *( mMsg + index );
            index += 1;
            
            len = mMsg[ index ];
            r += "\n\tWEATHER GRAPHICS: "; r += std::string ( mMsg + index, len );
            index += len;
            
            r += "\n\tELAPSED TIME (ms): "; r += ( uint32_t ) *( mMsg + index );
            index += 2;
            
        }; break;
        case ACSProtocol::ACSP_SET_SESSION_INFO:
        {
            r = "ACSP_SET_SESSION_INFO";
            
            r += "\n\tSESSION INDEX: "; r += (unsigned short) mMsg[ 1 ];
            int namelen = mMsg[ 2 ] * 4;
            r += "\n\tSESSION NAME: "; r += std::string ( mMsg + 3, namelen );
            r += "\n\tSESSION INDEX: "; r += (unsigned short) mMsg[ namelen + 4 ];
            r += "\n\tLAPS: "; r += ( uint32_t ) *( mMsg + namelen + 5 );
            r += "\n\tTIME (s): "; r += ( uint32_t ) *( mMsg + namelen + 9 );
            r += "\n\tWAIT TIME (s): "; r += ( uint32_t ) *( mMsg + namelen + 13 );
        }; break;
        case ACSProtocol::ACSP_VERSION:
        {
            r = "ACSP_VERSION";
            r += "\n\tPROTOCOL VERSION: "; r += ( int8_t ) mMsg[ 1 ];
        }; break;
    }
    
    return r;
}
