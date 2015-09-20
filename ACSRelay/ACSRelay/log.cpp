#include "log.h"

#include <iostream>
#include <time.h>

const std::string Log::DEFAULT_LOG_FILE =  "acsrelay.log.txt";
const enum Log::OutputLevel Log::DEFAULT_LOG_LEVEL = Log::OutputLevel::NORMAL;

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

void Log::OutputLevel ( const enum OutputLevel level )
{
    mInstance -> mLevel = level;
}