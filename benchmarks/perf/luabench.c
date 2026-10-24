/**
 * @file luabench.c
 * @brief 在官方 Lua VM 中执行性能脚本，仅补充宿主单调纳秒时钟。
 */
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <stdio.h>
#include <string.h>
_Static_assert(sizeof(lua_Integer) == 8, "Lua benchmark requires 64-bit integers");
_Static_assert(sizeof(lua_Number) == 8, "Lua benchmark requires double precision");
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

/**
 * @brief 向 Lua 返回单调时钟的纳秒读数。
 * @param state 接收一个整数返回值的 Lua 状态。
 * @return Lua 栈上的返回值数量，恒为一。
 */
static int now_ns(lua_State *state)
{
#ifdef _WIN32
    LARGE_INTEGER frequency, ticks;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&ticks);
    const lua_Integer value = (lua_Integer)(ticks.QuadPart / frequency.QuadPart) * 1000000000LL + (lua_Integer)((ticks.QuadPart % frequency.QuadPart) * 1000000000LL / frequency.QuadPart);
#else
    struct timespec ticks;
    clock_gettime(CLOCK_MONOTONIC, &ticks);
    const lua_Integer value = (lua_Integer)ticks.tv_sec * 1000000000LL + ticks.tv_nsec;
#endif
    lua_pushinteger(state, value);
    return 1;
}

/**
 * @brief 打印实际 Lua 版本，或在标准 VM 中执行一个性能脚本。
 * @param argc 参数数量，应为二。
 * @param argv 第二项为 --version 或脚本路径。
 * @return 成功返回零，参数、状态创建或脚本执行失败返回非零。
 */
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--version") == 0)
    {
        puts(LUA_RELEASE " (official VM, monotonic clock host)");
        return 0;
    }
    if (argc != 2)
    {
        fputs("usage: luabench script.lua\n", stderr);
        return 2;
    }
    lua_State *state = luaL_newstate();
    if (state == NULL) return 1;
    luaL_openlibs(state);
    lua_pushcfunction(state, now_ns);
    lua_setglobal(state, "now_ns");
    const int result = luaL_loadfile(state, argv[1]) || lua_pcall(state, 0, LUA_MULTRET, 0);
    if (result != 0) fprintf(stderr, "%s\n", lua_tostring(state, -1));
    lua_close(state);
    return result == 0 ? 0 : 1;
}
