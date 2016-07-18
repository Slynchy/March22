#include <engine/M22Engine.h>

lua_State *M22Lua::STATE;

int M22Lua::Initialize()
{
	char buff[256] = "print(\"[M22Lua*] Lua state initialized!\")";
	printf("[M22Lua] Initializing Lua...\n");
	M22Lua::STATE = luaL_newstate();
	luaL_openlibs(M22Lua::STATE);

	lua_register(M22Lua::STATE, "M22_ExecuteCommand", ExecuteM22ScriptCommand);
	luaL_loadbuffer(M22Lua::STATE, buff, strlen(buff), "line");
	lua_pcall(M22Lua::STATE, 0, 0, 0);
	return 0;
};

void M22Lua::Shutdown()
{
	lua_close(M22Lua::STATE);
	return;
};

int M22Lua::ExecuteM22ScriptCommand(lua_State* L)
{
	std::vector<std::wstring> temp;
	int num_of_arg = lua_gettop (L);
	M22Script::LINETYPE command_type;
	for(int i = 1; i <= num_of_arg; i++)
	{
		temp.push_back(M22Script::to_wstring(lua_tostring(L, i)));
	};

	command_type = M22Script::CheckLineType(temp.at(0));

	M22ScriptCompiler::line_c templineC;
	templineC.m_lineType = command_type;
	M22ScriptCompiler::CompileLine(templineC, temp);

	// Push the results of the script command
	lua_pushinteger(L, M22ScriptCompiler::ExecuteCommand( templineC, M22Script::currentLineIndex));
	return 1;
}