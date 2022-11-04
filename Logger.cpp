#include "Logger.h"

#include <errno.h>
#include <stdexcept>


using namespace bin::demo;


const char * Logger::m_scLevel[ LEVEL_COUNT ]{
	"ALL",
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL",
	"OFF"
};

Logger * Logger::m_sInstance = NULL;

Logger::Logger( ) : m_level( LEVEL_INFO ), m_len( 0 ), m_maxLen( 0 ), m_fileName("Logger.log" )
{
	std::cout << "(bin::utility) begin logging" << std::endl;
}

Logger::~Logger( )
{
	std::cout << "(bin::utility) Log end!" << std::endl;
	m_sInstance->close( );
}

Logger * Logger::instance( )
{
	if ( m_sInstance == NULL )
	{
		m_sInstance = new Logger( );
	}

	return m_sInstance;
}

void Logger::open( const string & fileName )
{
	m_fileName = fileName;

	m_fOut.open( fileName, ios::app );
	if ( m_fOut.fail( ) )
	{
		throw std::logic_error( "failed to open file " + fileName );
	}
	m_fOut.seekp( 0, ios::end );
	//m_fOut.seekp( ios::end );
	m_len = m_fOut.tellp( );
}

void Logger::open( const string & fileName, int fileMaxLen )
{
	open( fileName );
	setFile_maxLen( fileMaxLen );
}

void Logger::close( )
{
	m_fOut.close( );
}

void Logger::setFile_maxLen( int bytes )
{
	m_maxLen = bytes;
}

void Logger::level( int level )
{
	m_level = level;
}

void Logger::log( Level level, const char * file, const int line, const char * format, ... )
{
	/*if ( m_level > level )
	{
		return;
	}*/

	if ( m_fOut.fail( ) )
	{
		throw std::logic_error( "failed to open file " + m_fileName );
	}

	// time formatting
	time_t now;
	time( &now );
	struct tm ptm{ 0 };
	localtime_s( &ptm, &now );
	char timestamp[ 24 ];
	memset( timestamp, 0, sizeof( timestamp ) );
	strftime( timestamp, sizeof( timestamp ), ".%Y-%m-%d_%H-%M-%S", &ptm );

	// output formatting
	//2022-10-10 10:10:10->DEBUG    @ Logger.cpp: 35
	const char * fmt = "[%s][%s][%s: %d] =》";
	int len = snprintf( NULL, 0, fmt, timestamp, m_scLevel[m_level], file, line );
	if ( len > 0 )
	{
		char * buffer = new char[ len + 1 ];
		snprintf( buffer, len + 1, fmt, timestamp, m_scLevel[ m_level ], file, line );
		buffer[ len ] = 0; // '\0'
		m_fOut << buffer; // 写入流

		std::cout << "\n" << buffer;

		delete[ ] buffer;
		m_len += len;
	}

	// 获取格式化内容长度
	va_list arg_ptr;
	va_start( arg_ptr, format );
	int size = 0;
	size = vsnprintf( NULL, 0, format, arg_ptr );
	va_end( arg_ptr );
	if ( size > 0 )
	{
		char * content = new char[ size + 1 ];
		va_start( arg_ptr, format );
		size = vsnprintf( content, size + 1, format, arg_ptr );
		va_end( arg_ptr );
		m_fOut << content;
		std::cout << content;
		delete[ ] content;
	}
	m_fOut << "\n";
	m_len += size;

	m_fOut.flush( ); // 写入文件

	if ( m_maxLen > 0 && m_len >= m_maxLen )
	{
		rotate( );
	}
}

void Logger::rotate( )
{
	// 关闭文件流
	close( );

	// 建立新文件
	time_t now;
	time( &now );
	struct tm ptm;
	localtime_s( &ptm, &now );
	char timestamp[ 24 ];
	memset( timestamp, 0, sizeof( timestamp ) );
	strftime( timestamp, sizeof( timestamp ), ".%Y%m%d-%H%M%S", &ptm );
	string filename = m_fileName + timestamp;

	// 文件内容转移到新文件
	if ( rename( m_fileName.c_str( ), filename.c_str( ) ) != 0 )
	{
		/*size_t errmsglen = strerrorlen_s( errno ) + 1;
		char errmsg[ errmsglen ];
		strerror_s( errmsg, errmsglen, errno );*/
		throw std::logic_error( "rename log file failed !" );
		//throw std::logic_error( "rename log file failed: " + string( strerror( errno ) ) );

	}

	// 打开原文件
	m_fOut.open( m_fileName );
}