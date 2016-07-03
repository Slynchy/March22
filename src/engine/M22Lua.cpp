#include "../M22Engine.h"

lua_State *M22Lua::STATE;

int M22Lua::Initialize()
{
	char buff[256] = "print(\"[M22Lua*] Lua state initialized!\")";
	printf("[M22Lua] Initializing Lua...\n");
	M22Lua::STATE = luaL_newstate();
	luaL_openlibs(M22Lua::STATE);

	luaL_loadbuffer(M22Lua::STATE, buff, strlen(buff), "line");
	lua_pcall(M22Lua::STATE, 0, 0, 0);
	return 0;
};

void M22Lua::Shutdown()
{
	lua_close(M22Lua::STATE);
	return;
};