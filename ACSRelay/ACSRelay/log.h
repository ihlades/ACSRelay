#ifndef __LOG_H
#define __LOG_H

#include <fstream>
#include <string>

class Log
{
public:
    enum OutputLevel
    {
        ERROR = 0,
        WARNING = 1,
        VERBOSE = 2,
        NORMAL = 3,
        DEBUG = 4
    };
    
    static void Start ();
    static void Start ( const enum OutputLevel level, const std::string logfile );
    static void Stop ();
    
    static Log& d ();
    static Log& e ();
    static Log& i ();
    static Log& v ();
    static Log& w ();
    
    Log& operator<< ( const std::string &log );
    Log& operator<< ( const long long &log );
    Log& operator<< ( const unsigned long long &log );
    Log& operator<< ( const double &log );
    Log& operator<< ( const bool &log );
    
    Log& operator<< ( const long &log ) { return *this << ( long long ) log; }
    Log& operator<< ( const int &log ) { return *this << ( long long ) log; }
    Log& operator<< ( const short &log ) { return *this << ( long long ) log; }
    
    Log& operator<< ( const unsigned long &log ) { return *this << ( unsigned long long ) log; }
    Log& operator<< ( const unsigned int &log ) { return *this << ( unsigned long long ) log; }
    Log& operator<< ( const unsigned short &log ) { return *this << ( unsigned long long ) log; }
    
    const static enum OutputLevel DEFAULT_LOG_LEVEL;
    const static std::string DEFAULT_LOG_FILE;
    
    static OutputLevel OutputLevel ()
    {
        return mLevel;
    }
    
private:
    Log ();
    Log ( const enum OutputLevel level, const std::string logfile );
    ~Log ();
    
    static void OutputLevel ( const enum OutputLevel level );
    
    static Log* mInstance;
    
    static std::ofstream *mLogFile;
    
    static std::string mLogFilename;
    static bool mFileOutputEnabled;
    static enum OutputLevel mLevel;
    static enum OutputLevel mRequestedLevel;
    
};

#endif // __LOG_H