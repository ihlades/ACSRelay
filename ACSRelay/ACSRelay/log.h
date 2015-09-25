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

#ifndef __LOG_H
#define __LOG_H

#include <fstream>
#include <string.h>

/**
 * @class LogPacket
 * @brief Used to log ACSP packets.
 * Helper class used to log ACSP packets using the
 * standard Log functions and operators.
 */
class LogPacket
{
public:
    /**
     * @brief LogPacket constructor.
     * @param msg Packet data as a byte array.
     * @param len Packet size (equal to the byte array's size).
     */
    LogPacket ( char* msg, long len ) { mMsg = strndup ( msg, len ); mLen = len; }
    /**
     * @brief Returns packet data in a readable format.
     * @return A string describing the packet.
     */
    std::string ToString () const;
private:
    char *mMsg;
    long mLen;
    
    
};

/**
 * @class Log
 * @brief Used to output log messages to screen and the log file.
 */
class Log
{
public:
    /**
     * @brief Possible levels of verbosity for the log functions.
     */
    enum OutputLevel
    {
        ERROR = 0, ///< Output level corresponding to error messages.
        WARNING = 1, ///< Output level corresponding to warning messages.
        VERBOSE = 2, ///< Output level corresponding to more verbose messages.
        NORMAL = 3, ///< Output level corresponding to usual informative messages.
        DEBUG = 4 /*!< Output level corresponding to debug messages. If the output
                       level is set to this, all types of messages will be printed. */
    };

    /**
     * @brief Starts the log.
     */
    static void Start ();
    /**
     * @brief Starts the log.
     * @param level Desired output level.
     * @param logfile Filename of the log file in which to output messages.
     */
    static void Start ( const enum OutputLevel level, const std::string logfile );
    /**
     * @brief Stops the log.
     */
    static void Stop ();

    /**
     * @brief Used to output debugging messages.
     * @return Log object ready to output debugging messages.
     */
    static Log& d ();
    /**
     * @brief Used to output error messages.
     * @return Log object ready to output error messages.
     */
    static Log& e ();
    /**
     * @brief Used to output general information messages.
     * @return Log object ready to output general information messages.
     */
    static Log& i ();
    /**
     * @brief Used to output more verbose messages.
     * @return Log object ready to output more verbose messages.
     */
    static Log& v ();
    /**
     * @brief Used to output warning messages.
     * @return Log object ready to output warning messages.
     */
    static Log& w ();

    /**
     * @brief Used to output strings to the log.
     * @param log String to output in the log.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const std::string &log );
    /**
     * @brief Used to output character arrays to the log.
     * This calls the string output operator.
     * @param log Character array to output in the log.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const char *log ) { return *this << ( std::string ( log ) ); }

    /**
     * @brief Used to output long integers to the log.
     * @param log Long integer to output in the log.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const long &log );
    /**
     * @brief Used to output integers to the log.
     * This calls the long integer output operator.
     * @param log Integer to output in the log.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const int &log ) { return *this << static_cast<long>(log); }
    /**
     * @brief Used to output short integers to the log.
     * This calls the long integer output operator.
     * @param log Short nteger to output in the log.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const short &log ) { return *this << static_cast<long>(log); }

    /**
     * @brief Used to output unsigned long integers to the log.
     * @param log Unsigned long integer to output in the log.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const unsigned long &log );
    /**
     * @brief Used to output unsigned integers to the log.
     * This calls the unsigned long integer output operator.
     * @param log Unsigned integer to output in the log.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const unsigned int &log ) { return *this << static_cast<unsigned long>(log); }
    /**
     * @brief Used to output unsigned short integers to the log.
     * This calls the unsigned long integer output operator.
     * @param log Unsigned short integer to output in the log.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const unsigned short &log ) { return *this << static_cast<unsigned long>(log); }

    /**
     * @brief Used to output floating point numbers to the log.
     * @param log Double floating point number to output in the log.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const double &log );
    /**
     * @brief Used to output boolean values to the log.
     * This outputs "TRUE" or "FALSE" depending on the parameter value.
     * @param log Boolean value to output in the log.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const bool &log );

    /**
     * @brief Used to output ACSP packet data to the log.
     * @param packet ACSP packet to output in the log as a LogPacket object.
     * @return Log object that was used to perform the task.
     */
    Log& operator<< ( const LogPacket& packet ) { *this << packet.ToString(); return *this; }

    /**
     * @brief The default verbosity of the log functions.
     */
    const static enum OutputLevel DEFAULT_LOG_LEVEL;
    /**
     * @brief The default filename of the log file.
     */
    const static std::string DEFAULT_LOG_FILE;
    
private:
    Log ();
    Log ( const enum OutputLevel level, const std::string logfile );
    ~Log ();
    
    static void SetOutputLevel ( const enum OutputLevel level );
    
    static Log* mInstance;
    
    std::ostream *mOutput;
    std::ofstream *mLogFile;
    
    enum OutputLevel mLevel;
    enum OutputLevel mRequestedLevel;
    
    std::string mLogFilename;
    bool mFileOutputEnabled;
    
    bool mTreatWarningsAsErrors;
};

#endif // __LOG_H
