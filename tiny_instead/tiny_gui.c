#include "externals.h"
#include "internals.h"
static int game_grab_events = 0;
static int game_wait_use = 1;
static char* GetClipboardText()
{
	if (!IsClipboardFormatAvailable(CF_TEXT) || !OpenClipboard(NULL)) return NULL;
	char* buf;
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData != NULL) {
		buf = GlobalLock(hData);
		GlobalUnlock(hData);
	}
	CloseClipboard();
		return buf;
}
static BOOL SetClipboardText(char* buf) {
	if (!OpenClipboard(NULL)) return FALSE;
		EmptyClipboard();
		const size_t len = strlen(buf) + 1;
		HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, len);
		if (hglbCopy == NULL) {
			CloseClipboard();
			return FALSE;
}
		memcpy(GlobalLock(hglbCopy), buf, len);
		GlobalUnlock(hglbCopy);
		SetClipboardData(CF_TEXT, hglbCopy);
	return TRUE;
}
static int luaB_clipboard(lua_State* L) {
const char* text = luaL_optstring(L, 1, NULL);
	if (!text) {
		char* buf = GetClipboardText();
		if (buf) {
			lua_pushstring(L, buf);
			//free(buf);
}
		else lua_pushboolean(L, 0);
		return 1;
	}
	else {
		lua_pushboolean(L, SetClipboardText(text));
//free(text);
		return 1;
}
	return 0;
	}
static int luaB_wait_use(lua_State* L) {
	return 0;
}
static int luaB_grab_events(lua_State* L) {
	return 0;
}
static int luaB_text_input(lua_State* L) {
	return 0;
}

static const luaL_Reg gui_funcs[] = {
	{ "instead_clipboard", luaB_clipboard},
	{ "instead_wait_use", luaB_wait_use },
	{ "instead_grab_events", luaB_grab_events},
	{ "instead_text_input", luaB_text_input},
	{NULL, NULL}
};
static int gui_init(void)
{
	instead_api_register(gui_funcs);
	//input_text_state = input_text(-1);
	game_wait_use = 1;
	game_grab_events = 0;
/*
char path[PATH_MAX];
snprintf(path, sizeof(path), "%s/%s", instead_stead_path(), "/ext/gui.lua");
	return instead_loadfile(dirpath(path));*/
return 0;
}
static int gui_done(void)
{
//input_text(input_text_state);
	return 0;
}

static int gui_cmd(void)
{
//Заготовка для отображения картинок.
	instead_function("instead.get_picture", NULL);
	char* pict = instead_retval(0);
	instead_clear();
	instead_unlock();
	unix_path(pict);
if(pict) free(pict);
	return 0;
}

static struct instead_ext ext = {
	.init = gui_init,
.cmd =gui_cmd,
	.done = gui_done,
};

int instead_gui_init(void)
{
	return instead_extension(&ext);
}
