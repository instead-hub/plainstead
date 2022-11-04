#include "externals.h"
#include "internals.h"
int need_restart = 0;
int need_load = 0;
int need_save = 0;
void restart();
void save();
void load();
static int luaB_menu(lua_State* L)
{
	const char* menu = luaL_optstring(L, 1, NULL);
	if (!menu)
		return 0;
	need_save = !strcmp(menu, "save");
	need_load = !strcmp(menu, "load");
	if (need_save) save(); else if (need_load) load();
	return 0;
}

static int luaB_restart(lua_State* L)
{
	need_restart = !lua_isboolean(L, 1) || lua_toboolean(L, 1);
	if (need_restart) restart();
	return 0;
}

static const luaL_Reg tiny_funcs[] = {
	{ "instead_restart", luaB_restart },
	{ "instead_menu", luaB_menu },
	{ NULL, NULL }
};

static int parser_init(void)
{
return instead_api_register(tiny_funcs);
}
static struct instead_ext ext = {
	.init = parser_init,
};
int instead_metaparser_init() {
	return instead_extension(&ext);
}