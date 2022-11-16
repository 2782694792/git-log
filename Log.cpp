#pragma once

#include "Log.h"

#define  LOGINIT LOG::getInstance( )
#define TIMEBUFFERLEN 32
#define MAXCONTENT MAXCHAR

using namespace bin::log;

LOG * LOG::m_log = NULL;
std::string LOG::m_logBuffer = " ";
HANDLE LOG::m_fileHandle = INVALID_HANDLE_VALUE;
std::mutex LOG::m_logMutex;
// �ٽ�����ÿ���߳��з����ٽ���Դ���Ƕδ��룬������Ӳ����Դ������̱߳��뻥��ض������з���
CRITICAL_SECTION LOG::criticalSection;



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



static int printfToBuffer( char * buffer, int size, char * format, ... )
{
	va_list ap;
	va_start( ap, format );
	int ret = vsnprintf( buffer, size, format, ap );
	va_end( ap );
	return ret;
}

static int getSysTime( char * timeFormatHead, char * timeFormatContent )
{
	if ( timeFormatHead == NULL && timeFormatContent == NULL )
	{
		return -1;
	}
	else
	{
		char * timeOutput;
		timeOutput = timeFormatHead;
		SYSTEMTIME localTime;
		GetLocalTime( &localTime );
		int ret = -1;
		if ( timeFormatHead != NULL )
		{
			char * format = (char *) ( "%04d%02d%02d-%02d%02d%02d.%05d" ); // 1999-19-09 09:19:09.59954
			ret = printfToBuffer( timeFormatHead, 128, format
								  , localTime.wYear
								  , localTime.wMonth
								  , localTime.wDay
								  , localTime.wHour
								  , localTime.wMinute
								  , localTime.wSecond
								  , localTime.wMilliseconds );
		}
		if ( timeFormatContent != NULL )
		{
			timeOutput = timeFormatContent;
			char * format = (char *) ( "[%04d-%02d-%02d %02d:%02d:%02d.%05d]" ); // 1999-19-09 09:19:09.59954
			ret = printfToBuffer( timeFormatContent, 128, format
								  , localTime.wYear
								  , localTime.wMonth
								  , localTime.wDay
								  , localTime.wHour
								  , localTime.wMinute
								  , localTime.wSecond
								  , localTime.wMilliseconds );
		}
		return ret;
	}
}



LOG::LOG( )
// : m_maxLen( 128 )
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
	uninit( );
}


void LOG::setFileMaxLen( int bytes )
{
	this->m_maxLen = bytes;
}


int LOG::getFileMaxLen( )
{
	return this->m_maxLen;
}


/**
 * Description : ��ʼ���ȼ������
 * Author : ZhengShuoBin
 * Param :  [in]  loglevel  �ж�Ĭ������ĵȼ�����
 * Param :  [in]  logtarget  target �ж����
 * Others :
 **/
void LOG::init( LOGLEVEL loglevel, LOGTARGET logtarget )
{
	setLogLevel( loglevel );
	setLogTarget( logtarget );
	InitializeCriticalSection( &criticalSection );
}

void LOG::uninit( )
{
	if ( INVALID_HANDLE_VALUE != m_fileHandle )
	{
		CloseHandle( m_fileHandle );
	}
	DeleteCriticalSection( &criticalSection );
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
	// ��ǰ·��
	TCHAR fileDirectory[ _MAX_DIR ];
	GetCurrentDirectory( _MAX_DIR, fileDirectory );

	// log �ļ�·��
	TCHAR logFileDirectory[ _MAX_DIR ];
	_stprintf_s( logFileDirectory, _T( "%s\\Log1Bin\\" ), fileDirectory );
	if ( _taccess( logFileDirectory, 0 ) == -1 ) // ȷ���ļ����ļ��еķ���Ȩ��  _access  ��#include <io.h> ��
	{
		_tmkdir( logFileDirectory ); // �ļ��в��������´���
	}

	// fixme: Ŀ¼·�����ļ�
	//wchar_t * moduleFileName;
	//GetModuleFileName( NULL, moduleFileName, MAX_PATH ); // ����ǰĿ¼
	WCHAR moduleFileName[ MAX_PATH ];
	GetModuleFileName( NULL, moduleFileName, MAX_PATH );
	wchar_t c = ( '\\' );
	PWCHAR p = wcsrchr( moduleFileName, c );     // ����ָ��·���� \ �ַ������һ��λ�õ�ָ��
	p++;
	for ( int i = _tcslen( p ); i > 0; i-- ) // _t  �괦��ȡ�����ַ�������ӳ���ַ����������ַ����ĳ����ֽ�
	{
		if ( p[ i ] == _T( '.' ) ) // fixme: ��׺������
		{
			// ��ȡ�������ƣ��޺�׺
			p[ i ] = _T( '\0' );
			break;
		}
	}

	WCHAR logFileName[ MAX_PATH ];
	char timeBuffer[ TIMEBUFFERLEN ];
	//memset( timeBuffer, 0, sizeof( timeBuffer ) );
	int ret = getSysTime( timeBuffer, NULL );
	if ( ret == -1 )
	{
		return -1;
	}
	USES_CONVERSION;
	_stprintf_s( logFileName, _T( "%s%s_%d_%s" ), logFileDirectory, p, GetCurrentProcessId( ), A2W(timeBuffer) );
	//_stprintf_s( logFileName, _T( "%s%s_%d_%s" ), logFileDirectory, p, GetCurrentProcessId( ), A2W( timeBuffer ) );
	DWORD dwNum = WideCharToMultiByte( CP_OEMCP, NULL, logFileName, -1, NULL, 0, NULL, FALSE );
	// psText Ϊ char* ����ʱ���飬��Ϊ��ֵ�� std::string ���м����
	char * psText = new char[ dwNum ];
	if ( psText == NULL )
	{
		delete[ ]psText;
	}
	else
	{
		WideCharToMultiByte( CP_OEMCP, NULL, logFileName, -1, psText, dwNum, NULL, FALSE );
		m_log->m_fileName = psText;
		delete[ ]psText;
	}

	m_fileHandle = CreateFile(
		logFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS, // CREATE_NEW
		FILE_ATTRIBUTE_ARCHIVE,
		NULL );
	//int i = GetLastError( );
	if ( INVALID_HANDLE_VALUE == m_fileHandle )
	{
		std::cout << "failed to create file" << std::endl;
		return -1;
	}

	return 0;
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
	char timeBuffer[ TIMEBUFFERLEN ];
	//memset( timeBuffer, 0, sizeof( timeBuffer ) );
	int ret = getSysTime( NULL, timeBuffer );
	if ( ret == -1 )
	{
		return -1;
	}
	m_logBuffer += std::string( timeBuffer );
	const char * logLevel = m_ccLogLevel[ loglevel ];

	// [���̺�][�̺߳�][Log����][�ļ���][������:�к�]
	char locInfo[ 128 ];
	//memset( locInfo, 0, sizeof( locInfo ) );
	char * format2 = (char *) ( "[PID:%4d][TID:%4d][%s][-%s][%s:%4d]" );
	int ret2 = printfToBuffer( locInfo, 128, format2
							   , GetCurrentProcessId( )
							   , GetCurrentThreadId( )
							   , logLevel
							   , filename
							   , function
							   , linenumber );
	m_logBuffer += std::string( locInfo );

	// describe content
	char content[ MAXCONTENT ];
	va_list ap;
	va_start( ap, format );
	vsnprintf_s( content, MAXCONTENT, format, ap );
	va_end( ap );
	m_logBuffer += std::string( content );
	m_logBuffer += "\n";

	// ����ļ� : Ĭ�ϵȼ� <= ָ���ȼ�
	if ( m_log->m_logLevel <= loglevel )
	{
		m_log->outputToTarget( );
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
	if ( m_log->getLogTarget( ) & LOG_TARGET_FILE )
	{
		m_log->createFile( );
		SetFilePointer( m_fileHandle, 0, NULL, FILE_END );
		DWORD dwByteWritten = 0;
		WriteFile( m_fileHandle, m_logBuffer.c_str( ), m_logBuffer.length( ), &dwByteWritten, 0 );
		FlushFileBuffers( m_fileHandle );

		m_log->m_fileLen = GetFileSize( m_fileHandle, NULL );

		//if ( m_log->m_fileLen >= m_log->m_maxLen && m_log->m_maxLen > 0 )
		//{
		//	rotate( );
		//}
	}

	if ( m_log->getLogTarget( ) & LOG_TARGET_CONSOLE )
	{
		printf( "%s", m_logBuffer.c_str( ) );
	}

	m_logBuffer.clear( );
}

/*
void LOG::rotate( )
{
	CloseHandle( m_fileHandle );

	//char timeBuffer[ 128 ];
	//int ret = getSysTime( timeBuffer );
	time_t now;
	time( &now );
	struct tm ptm;
	localtime_s( &ptm, &now );
	char timeBuffer[ 24 ];
	memset( timeBuffer, 0, sizeof( timeBuffer ) );
	strftime( timeBuffer, sizeof( timeBuffer ), ".%Y%m%d-%H%M%S", &ptm );
	std::string filename = m_log->m_fileName + timeBuffer;
	// �ļ�����ת�Ƶ����ļ�
	if ( rename( m_log->m_fileName.c_str( ), filename.c_str( ) ) != 0 )
	{
		//throw std::logic_error( "rename log file failed: " + std::string( strerror( errno ) ) );
		return;
	}

	// ��ԭ�ļ�
	m_log->createFile( );
}
*/