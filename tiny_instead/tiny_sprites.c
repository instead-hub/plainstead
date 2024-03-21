#include "externals.h"
#include "internals.h"
#include <string.h>
static int callback_ref = 0;
static int render_callback_dirty = 0;
extern void updateText(char* text);
static int luaB_text_sprite(lua_State* L) {
	//const char* font = luaL_optstring(L, 1, NULL);
	const char* text = luaL_optstring(L, 2, NULL);
	//const char* color = luaL_optstring(L, 3, NULL);
	int style = luaL_optnumber(L, 4, 0);
	//const char* desc = luaL_optstring(L, 5, NULL);
	/*if (font) free(font);
	if (color ) free(color);
	if (desc ) free(desc);*/
		if (text) {
		updateText(text);
		//free(text);
	}
	lua_pushstring(L, "");
	return 1;
}
BOOL check_bool(BOOL condition) {
	if (!condition) {
		fprintf(stderr, "%s\n", condition);
		return FALSE;
	}
	return TRUE;
}
//Эти три функции для работы с экраном были взяты с https://stackoverflow.com/questions/70976583/get-real-screen-resolution-using-win32-api
void GetMonitorRealResolution(HMONITOR monitor, int* pixelsWidth, int* pixelsHeight)
{
	MONITORINFOEX info = { sizeof(MONITORINFOEX) };
	check_bool(GetMonitorInfo(monitor, &info));
	DEVMODE devmode;
	//ZeroMemory(&devmode, sizeof(devmode));
	devmode.dmSize = sizeof(DEVMODE);
	check_bool(EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode));
	*pixelsWidth = devmode.dmPelsWidth;
	*pixelsHeight = devmode.dmPelsHeight;
}
float GetMonitorScalingRatio(HMONITOR monitor)
{
	MONITORINFOEX info = { sizeof(MONITORINFOEX) };
	check_bool(GetMonitorInfo(monitor, &info));
	DEVMODE devmode;
	//ZeroMemory(&devmode, sizeof(devmode));
	devmode.dmSize = sizeof(DEVMODE);
	check_bool(EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode));
	return (info.rcMonitor.right - info.rcMonitor.left) / (float)devmode.dmPelsWidth;
}
float GetRealDpiForMonitor(HMONITOR monitor)
{
	return /*GetDpiForSystem() / */ 96.0 / GetMonitorScalingRatio(monitor);
}
static int luaB_screen_size(lua_State* L) {
	/*const long width = GetSystemMetrics(SM_CXSCREEN);
	const long height = GetSystemMetrics(SM_CYSCREEN);*/
HMONITOR monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	int monitor_width = info.rcMonitor.right - info.rcMonitor.left;
	int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;
	/*RECT windowsize;
	GetClientRect(GetDesktopWindow(), &windowsize);
	lua_pushinteger(L	,windowsize.right - windowsize.left);
	lua_pushinteger(L,windowsize.bottom - windowsize.top);*/
	lua_pushinteger(L, monitor_width);
	lua_pushinteger(L, monitor_height);
	return 2;
}
static const luaL_Reg sprites_funcs[] = {
{
"instead_sprite_text", luaB_text_sprite},
	{"instead_screen_size",luaB_screen_size},
{NULL, NULL}
	};
static int sprites_init(void) {
	char path[1024];
/*
	if (pixels_create_meta(instead_lua()))
		return -1;
	*/
	snprintf(path, sizeof(path), "%s/%s", instead_stead_path(), "/ext/sprites.lua");
instead_api_register(sprites_funcs);
//return instead_loadfile(dirpath(path));
return 0;
}
static void sprites_free(void)
{

}
static int sprites_done(void)
{
	if (callback_ref) {
		luaL_unref(instead_lua(), LUA_REGISTRYINDEX, callback_ref);
		callback_ref = 0;
		render_callback_dirty = 0;
	}
	sprites_free();
	return 0;
}
static int sprites_err(void)
{
	if (callback_ref) {
		luaL_unref(instead_lua(), LUA_REGISTRYINDEX, callback_ref);
		callback_ref = 0;
	}
	return 0;
}

static struct instead_ext ext ={
.init =sprites_init,.done =sprites_done,.err=sprites_err
};

int instead_sprites_init(void) {
	return instead_extension(&ext);
}