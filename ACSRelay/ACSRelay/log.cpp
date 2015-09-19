#include "log.h"

#include <iostream>

const std::string Log::DEFAULT_LOG_FILE =  "acsrelay.log.txt";
const enum Log::OutputLevel DEFAULT_LOG_LEVEL = Log::OutputLevel::NORMAL;

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
    
    mLogFile = new std::ofstream ( mLogFilename, std::ios::app );
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
    mRequestedLevel = OutputLevel::DEBUG;
    *mInstance << "\n";
    return *mInstance;
}

Log& Log::e ()
{
    mRequestedLevel = OutputLevel::ERROR;
    *mInstance << "\n";
    return *mInstance;
}

Log& Log::i ()
{
    mRequestedLevel = OutputLevel::NORMAL;
    *mInstance << "\n";
    return *mInstance;
}

Log& Log::v ()
{
    mRequestedLevel = OutputLevel::VERBOSE;
    *mInstance << "\n";
    return *mInstance;
}

Log& Log::w ()
{
    mRequestedLevel = OutputLevel::WARNING;
    *mInstance << "\n";
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

void Log::OutputLevel ( const enum OutputLevel level )
{
    mLevel = level;
}