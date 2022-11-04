#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <fstream>
#include <time.h>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>

using namespace std;

namespace bin
{
	namespace demo
	{

#define __all(format, ...) \
    Logger::instance()->log(Logger::LEVEL_ALL, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define __trace(format, ...) \
    Logger::instance()->log(Logger::LEVEL_TRACE, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define __debug(format, ...) \
    Logger::instance()->log(Logger::LEVEL_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define __info(format, ...) \
    Logger::instance()->log(Logger::LEVEL_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define __warn(format, ...) \
    Logger::instance()->log(Logger::LEVEL_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define __error(format, ...) \
    Logger::instance()->log(Logger::LEVEL_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define __fatal(format, ...) \
    Logger::instance()->log(Logger::LEVEL_FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define __off(format, ...) \
    Logger::instance()->log(Logger::LEVEL_OFF, __FILE__, __LINE__, format, ##__VA_ARGS__)


		class Logger
		{

		public:
			enum Level
			{
				LEVEL_ALL = 0,
				LEVEL_TRACE,
				LEVEL_DEBUG,
				LEVEL_INFO,
				LEVEL_WARN,
				LEVEL_ERROR,
				LEVEL_FATAL,
				LEVEL_OFF,
				LEVEL_COUNT
			};


			static Logger * instance( );

			void log( Level level, const char * file, const int line, const char * fornat, ... );

			void level( int level );

			void open( const string & fileName );
			void open( const string & fileName, int fileMaxLen );

			void close( );
			void setFile_maxLen( int bytes );

		private:
			Logger( );
			~Logger( );

			void rotate( );

		private:

			static Logger * m_sInstance;

			static const char * m_scLevel[ LEVEL_COUNT ];

			int m_level;

			string m_fileName;
			ofstream m_fOut;
			int m_len;
			int m_maxLen;


		};
	}
}

