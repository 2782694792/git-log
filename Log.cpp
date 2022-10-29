#pragma once

#include "Log.h"

LOG * LOG::m_log = NULL;
std::string LOG::m_logBuffer = " ";
HANDLE LOG::m_fileHandle = INVALID_HANDLE_VALUE;
std::mutex LOG::m_logMutex;
// 临界区：每个线程中访问临界资源的那段代码，无论软硬件资源，多个线程必须互斥地对它进行访问
CRITICAL_SECTION criticalSection;



const char * LOG::m_ccLogLevel[ LOG_LEVEL_COUNT ]{
	"ALL",
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL",
	"OFF"
};

LOG::LOG( )
{
	init( LOG_LEVEL_ALL, LOG_TARGET_FILE );
}

LOG::LOG( LOGLEVEL level, LOGTARGET target )
{
	init( level, target );
}

LOG::LOG( LOGTARGET target )
{
	init( LOG_LEVEL_ALL, target );
}

LOG::LOG( LOGLEVEL level )
{
	init( level, LOG_TARGET_FILE );
}

LOG::~LOG( )
{
	DeleteCriticalSection( &criticalSection );
}

/**
 * Description : 初始化等级与输出
 * Author : ZhengShuoBin
 * Param :  [in]  loglevel  判断默认输出的等级条件
 * Param :  [in]  logtarget  target 判断输出
 * Others :
 **/
void LOG::init( LOGLEVEL loglevel, LOGTARGET logtarget )
{
	setLogLevel( loglevel );
	setLogTarget( logtarget );
	createFile( );
}

void LOG::uninit( )
{
	if ( INVALID_HANDLE_VALUE != m_fileHandle )
	{
		CloseHandle( m_fileHandle );
	}
}

LOG * LOG::getInstance( )
{
	if ( NULL == m_log )
	{
		m_logMutex.lock( );
		if ( NULL == m_log )
		{
			m_log = new LOG( );
		}
		m_logMutex.unlock( );
	}
	return m_log;
}

int LOG::getLogLevel( )
{
	return this->m_logLevel;
}

void LOG::setLogLevel( LOGLEVEL iLogLevel )
{
	this->m_logLevel = iLogLevel;
}

LOGTARGET LOG::getLogTarget( )
{
	return this->m_logTarget;
}

void LOG::setLogTarget( LOGTARGET iLogTarget )
{
	this->m_logTarget = iLogTarget;
}

int LOG::createFile( )
{
	// 当前路径
	TCHAR fileDirectory[ _MAX_DIR ];
	GetCurrentDirectory( _MAX_DIR, fileDirectory );

	// log 文件路径
	TCHAR logFileDirectory[ _MAX_DIR ];
	_stprintf_s( logFileDirectory, _T( "%s\\Log\\" ), fileDirectory );
	if ( _taccess( logFileDirectory, 0 ) == -1 ) // 确定文件或文件夹的访问权限  _access  《#include <io.h> 》
	{
		_tmkdir( logFileDirectory ); // 文件夹不存在重新创建
	}

	// fixme: 目录路径与文件
	//wchar_t * moduleFileName;
	//GetModuleFileName( NULL, moduleFileName, MAX_PATH ); // 程序当前目录
	WCHAR moduleFileName[ MAX_PATH ];
	GetModuleFileName( NULL, moduleFileName, MAX_PATH );
	wchar_t c = ( '\\' );
	PWCHAR p = wcsrchr( moduleFileName, c );     // 返回指向路径中 \ 字符的最后一个位置的指针
	p++;
	for ( int i = _tcslen( p ); i > 0; i-- ) // _t  宏处理（取决于字符集），映射字符，返回中字符串的长度字节
	{
		if ( p[ i ] == _T( '.' ) ) // fixme: 后缀名处理
		{
			p[ i ] = _T( '\0' );
			break;
		}
	}

	WCHAR logFileName[ MAX_PATH ];
	_stprintf_s( logFileName, _T( "%s%s.log" ), logFileDirectory, p );

	m_fileHandle = CreateFile(
		logFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL );
	if ( INVALID_HANDLE_VALUE == m_fileHandle )
	{
		return -1;
	}
	return 0;
}

static int printfToBuffer( char * buffer, int size, char * format, ... )
{
	va_list ap;
	va_start( ap, format );
	int ret = vsnprintf( buffer, 128, format, ap );
	va_end( ap );
	return ret;
}

static int getSysTime( char * timeBuffer )
{
	if ( !timeBuffer )
	{
		return -1;
	}
	SYSTEMTIME localTime;
	GetLocalTime( &localTime );
	char * format = (char *) ( "[%04d-%02d-%02d %02d:%02d:%02d.%03d]" ); // 1999-19-09 09:19:09.599
	int ret = printfToBuffer( timeBuffer, 128, format
							  , localTime.wYear
							  , localTime.wMonth
							  , localTime.wDay
							  , localTime.wHour
							  , localTime.wMinute
							  , localTime.wSecond
							  , localTime.wMilliseconds );
	return ret;
}

int LOG::writeLog(
	LOGLEVEL loglevel,
	const char * filename,
	const char * function,
	int linenumber,
	const char * format,
	... )
{
	EnterCriticalSection( &criticalSection );

	// yyyy-mm-dd HH:MM:SS.sss
	char timeBuffer[ 128 ];
	int ret = getSysTime( timeBuffer );
	m_logBuffer += std::string( timeBuffer );

	const char * logLevel = m_ccLogLevel[ loglevel ];

	// [进程号][线程号][Log级别][文件名][函数名:行号]
	char locInfo[ 128 ];
	char * format2 = (char *) ( "[PID:%4d][TID:%4d][%s][-%s][%s:%4d]" );
	int ret2 = printfToBuffer( locInfo, 128, format2
							   , GetCurrentProcessId
							   , GetCurrentThreadId
							   , logLevel
							   , filename
							   , function
							   , linenumber );
	m_logBuffer += std::string( locInfo );

	// describe content
	char content[ 256 ];
	va_list ap;
	va_start( ap, format );
	vsnprintf_s( content, 256, format, ap );
	va_end( ap );

	m_logBuffer += std::string( content );
	m_logBuffer += "\n";

	// 输出文件 : 默认等级 <= 指定等级
	if ( m_log->m_logLevel <= loglevel )
	{
		outputToTarget( );
	}
	else
	{
		m_logBuffer.clear( );
	}
	LeaveCriticalSection( &criticalSection );

	return 0;
}

void LOG::outputToTarget( )
{
	if ( LOG::getInstance( )->getLogTarget( ) & LOG_TARGET_FILE )
	{
		SetFilePointer( m_fileHandle, 0, NULL, FILE_END );
		DWORD dwByteWritten = 0;
		WriteFile( m_fileHandle, m_logBuffer.c_str( ), m_logBuffer.length( ), &dwByteWritten, 0 );
		FlushFileBuffers( m_fileHandle );
	}

	if ( LOG::getInstance( )->getLogTarget( ) & LOG_TARGET_CONSOLE )
	{
		printf( "%s", m_logBuffer.c_str( ) );
	}

	m_logBuffer.clear( );
}