#ifndef __LOG_H__
#define __LOG_H__

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <mutex>
#include <string>
#include <atlconv.h>


#define FILENAME(x) strrchr(x,'\\') ? strrchr(x,'\\')+1:x

#define LOG_DEBUG(format, ...) LOG::getInstance()->writeLog(LOG_LEVEL_DEBUG, (FILENAME(__FILE__)) \
    , (__FUNCTION__), (__LINE__), format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)     LOG::getInstance()->writeLog(LOG_LEVEL_INFO, (FILENAME(__FILE__)) \
    , (__FUNCTION__), (__LINE__), format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...)    LOG::getInstance()->writeLog(LOG_LEVEL_WARN, (FILENAME(__FILE__)) \
    , (__FUNCTION__), (__LINE__), format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)    LOG::getInstance()->writeLog(LOG_LEVEL_ERROR, (FILENAME(__FILE__)) \
    , (__FUNCTION__), (__LINE__), format, ##__VA_ARGS__)
#define LOG_ALL(format, ...)    LOG::getInstance()->writeLog(LOG_LEVEL_ALL, (FILENAME(__FILE__)) \
    , (__FUNCTION__), (__LINE__), format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...)    LOG::getInstance()->writeLog(LOG_LEVEL_FATAL, (FILENAME(__FILE__)) \
    , (__FUNCTION__), (__LINE__), format, ##__VA_ARGS__)
#define LOG_OFF(format, ...)    LOG::getInstance()->writeLog(LOG_LEVEL_OFF, (FILENAME(__FILE__)) \
    , (__FUNCTION__), (__LINE__), format, ##__VA_ARGS__)
#define LOG_TRACE(format, ...)    LOG::getInstance()->writeLog(LOG_LEVEL_TRACE, (FILENAME(__FILE__)) \
    , (__FUNCTION__), (__LINE__), format, ##__VA_ARGS__)


#define ENTER() LOG_INFO("enter")
#define EXIT()  LOG_INFO("exit")
#define FAIL()  LOG_ERROR("fail")




enum LOGLEVEL
{
    LOG_LEVEL_ALL,
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_OFF,
    LOG_LEVEL_COUNT
};


enum LOGTARGET
{
    LOG_TARGET_NONE = 0x00,
    LOG_TARGET_CONSOLE = 0x01,
    LOG_TARGET_FILE = 0x10
};


/**
 * Company : OPT
 * FileName : Log.h
 * Author :  ZhengShuoBin
 * Version : V1.0.0
 * Date : 2022-10-29
 * Description : 日志单例类
 * Other :
 * ---- 1. 单例获取 ： getInstance( )
 * ---- 2. 日记翻滚 ： rotate( )
 */
class LOG
{
public:

    void init( LOGLEVEL loglevel, LOGTARGET logtarget );
    void uninit( );

    int createFile( );

    static LOG * getInstance( );

    int getLogLevel( );
    void setLogLevel( LOGLEVEL loglevel );

    LOGTARGET getLogTarget( );
    void setLogTarget( LOGTARGET logtarget );

    static int writeLog(
        LOGLEVEL loglevel,
        const char * filename,
        const char * function,
        int linenumber,
        const char * format,
        ...
    );

    //void setFileName( std::string name );
    //std::string getFileName( );

    void setFileMaxLen( int bytes );
    int getFileMaxLen( );

    //void setFileLen( int bytes );
    //int getFileLen( );

    static void outputToTarget( );

private:
    LOG( );
    LOG( LOGLEVEL level, LOGTARGET target );
    LOG( LOGTARGET target );
    LOG( LOGLEVEL level );
    ~LOG( );

    static const char * m_ccLogLevel[ LOG_LEVEL_COUNT ];
    static void rotate( );

    static LOG * m_log;

    // 互斥锁
    static std::mutex m_logMutex;

    static std::string m_logBuffer;

    static CRITICAL_SECTION criticalSection;

    LOGLEVEL m_logLevel;

    LOGTARGET m_logTarget;

    static HANDLE m_fileHandle;

    std::string m_fileName;
    int m_fileLen;
    int m_maxLen;

};


#endif