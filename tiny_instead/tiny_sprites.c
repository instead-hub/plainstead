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
static const luaL_Reg sprites_funcs[] = {
{
"instead_sprite_text", luaB_text_sprite},
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