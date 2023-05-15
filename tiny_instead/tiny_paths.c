#include "externals.h"
#include "internals.h"
extern void getGamePath(char* path);
extern void getSavePath(char* savepath);
extern void getAutoSavePath(char* autosavepath);
static int luaB_get_savepath(lua_State *L) {
	char savepath[MAX_PATH];
	getSavePath(savepath);
	//MessageBoxA(0, savepath, "", 0);
		unix_path(savepath);
			lua_pushstring(L, savepath);
	//free(savepath);
	return 1;
}
static int luaB_get_autosavepath(lua_State* L) {
	char autosavepath[MAX_PATH];
	getAutoSavePath(autosavepath);
	//MessageBoxA(0, savepath, "", 0);
	unix_path(autosavepath);
	lua_pushstring(L, autosavepath);
	//free(autosavepath);
	return 1;
}
static int luaB_get_exepath(lua_State* L) {
		lua_pushstring(L, instead_cwd());
		return 1;
	}

static int luaB_get_gamepath(lua_State* L) {
	char game_path[MAX_PATH];
	getGamePath(game_path);
	//strcat(game_path,"/");
	unix_path(game_path);
	lua_pushstring(L, game_path);
		//MessageBoxA(0, game_path, "", 0);
//free(game_path);
	return 1;
}

const luaL_Reg paths_funcs[] = {
{"instead_savepath", luaB_get_savepath},
	{"instead_autosavepath",luaB_get_autosavepath},
	{"instead_exepath", luaB_get_exepath},
	{"instead_gamepath",luaB_get_gamepath},
	{"instead_gamespath", luaB_get_gamepath},
	{ NULL, NULL }
};

static int paths_init(void)
{
	char path[PATH_MAX];
	snprintf(path, sizeof(path), "%s/%s", instead_stead_path(), "/ext/paths.lua");
	instead_api_register(paths_funcs);
	//return instead_loadfile(dirpath(path));
	return 0;
}

static struct instead_ext ext = {
	.init = paths_init,
};

int instead_paths_init(void)
{
	return instead_extension(&ext);
}
