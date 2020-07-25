#include "lua_tinker.h" // lua utility pakage

lua_State* L = 0;
int done=0;

//---------------------------------------------------------------------------------
int  MyModule_Fun0(int arg1, int arg2);
int  MyModule_Fun1(int arg1, int arg2);
int  MyModule_Fun2(int arg1, int arg2);
int  MyModule_Fun3(int arg1);
//---------------------------------------------------------------------------------
void MyModule_Do( char* pBatchFileName );
void MyModule_Exit(void);
//---------------------------------------------------------------------------------


void main( void )
{
	// Lua 를 초기화.
	L = lua_open();
	
	// Lua 기본 함수들을 로드.
	luaopen_base(L);

	// 함수 등록.
	lua_tinker::def(L, "MyModule_Fun0", MyModule_Fun0);
	lua_tinker::def(L, "MyModule_Fun1", MyModule_Fun1);
	lua_tinker::def(L, "MyModule_Fun2", MyModule_Fun2);
	lua_tinker::def(L, "MyModule_Fun3", MyModule_Fun3);
	lua_tinker::def(L, "batch"        , MyModule_Do  );
	lua_tinker::def(L, "exit"         , MyModule_Exit);

	// 
	done = 0;
	while( ! done )
	{
		char script_string[1024];
		if( gets( script_string ) )
		{			
			lua_tinker::dostring(L, script_string);
		}
	}

	// 프로그램 종료
	lua_close(L);
}

//---------------------------------------------------------------------------------
int MyModule_Fun0(int arg1, int arg2)
{
	printf( "--> MyModule_Fun0 is called: %d + %d = %d\n", arg1, arg2, arg1 + arg2 );
	return arg1 + arg2;
}
int MyModule_Fun1(int arg1, int arg2)
{
	printf( "--> MyModule_Fun1 is called: %d - %d = %d\n", arg1, arg2, arg1 - arg2 );
	return arg1 - arg2;
}
int MyModule_Fun2(int arg1, int arg2)
{
	printf( "--> MyModule_Fun2 is called: %d * %d = %d\n", arg1, arg2, arg1 * arg2 );
	return arg1 * arg2;
}
int MyModule_Fun3(int arg1)
{
	printf( "--> MyModule_Fun3 is called: %d ^ 2 = %d\n", arg1, arg1 * arg1 );
	return arg1 * arg1;
}
void MyModule_Do( char* pBatchFileName )
{
	printf( "--> MyModule_Do %s\n", pBatchFileName );
	lua_tinker::dofile(L, pBatchFileName );
}
void MyModule_Exit(void)
{
	printf( "--> MyModule_Exit\n" );
	done = 1;
}
