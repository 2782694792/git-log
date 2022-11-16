
#include <stdio.h>
#include "Logger.h"
#include "Log.h"

//using namespace bin::demo;
using namespace bin::log;

// 线程数量
#define THREAD_NUM 5
// 全局资源变量
int g_num = 0;

// 响应回调函数
unsigned int __stdcall func( void * pPM )
{
	//InitializeCriticalSection(&LOG:: )
	LOG_INFO( "enter" );
	Sleep( 50 );
	g_num++;
	LOG_INFO( "g_num = %d", g_num );
	LOG_INFO( "exit" );
	return 0;
}



int main( )
{
	//Logger::instance( )->open("Logger.log", 128 );
    //__debug( "1231322" );
    //LOG::getInstance( )->init( LOGLEVEL::LOG_LEVEL_ALL, LOGTARGET::LOG_TARGET_FILE );
	LOG::getInstance( )->init( LOGLEVEL::LOG_LEVEL_ALL, LOGTARGET::LOG_TARGET_FILE );
    LOG_ALL( "默认等级小于指定等级" );
	LOG_OFF( "默认等级 > 指定等级" );

	/// 线程句柄
	HANDLE handle[ THREAD_NUM ];

	// 线程编号
	int threadNum = 0;
	while ( threadNum < THREAD_NUM )
	{
		handle[ threadNum ] = (HANDLE)_beginthreadex( NULL, 0, func, NULL, 0, NULL );
		//! 等子线程接收到参数时，主线程可能改变了这个值
		threadNum++;
	}
	// 确保子线程已全部执行完毕
	WaitForMultipleObjects( THREAD_NUM, handle, true, INFINITE );


	return 0;
}