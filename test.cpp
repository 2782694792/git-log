
#include <stdio.h>
//#include "Logger.h"
#include "Log.h"

//using namespace bin::demo;

// �߳�����
#define THREAD_NUM 2
// ȫ����Դ����
int g_num = 0;

// ��Ӧ�ص�����
unsigned int __stdcall func( void * pPM )
{
	//LOG::getInstance( )->init( LOGLEVEL::LOG_LEVEL_FATAL, LOGTARGET::LOG_TARGET_CONSOLE );
	LOG_INFO( "enter" );
	Sleep( 50 );
	g_num++;
	LOG_INFO( "g_num = %d", g_num );
	LOG_INFO( "exit" );
	return 0;
}



int main( )
{
    //__debug( "1231322" );
    LOG::getInstance( )->init( LOGLEVEL::LOG_LEVEL_FATAL, LOGTARGET::LOG_TARGET_CONSOLE );
    LOG_ALL( "Ĭ�ϵȼ�С��ָ���ȼ�" );
	LOG_OFF( "Ĭ�ϵȼ� > ָ���ȼ�" );

	/// �߳̾��
	//HANDLE handle[ THREAD_NUM ];

	//// �̱߳��
	//int threadNum = 0;
	//while ( threadNum < THREAD_NUM )
	//{
	//	handle[ threadNum ] = (HANDLE)_beginthreadex( NULL, 0, func, NULL, 0, NULL );
	//	//! �����߳̽��յ�����ʱ�����߳̿��ܸı������ֵ
	//	threadNum++;
	//}
	//// ȷ�����߳���ȫ��ִ�����
	//WaitForMultipleObjects( THREAD_NUM, handle, true, INFINITE );


	return 0;
}