#include "externals.h"
#include "internals.h"
extern char* getGamePath();
extern char* getSavePath();
static int luaB_get_savepath(lua_State *L) {
	char* save = getSavePath();
		//unix_path(save);
	lua_pushstring(L, save);
	free(save);
	return 1;
}

static int luaB_get_exepath(lua_State* L) {
	char* instead_exec = getGamePath();
	strcat(instead_exec, "../..");
	//unix_path(instead_exec);
	lua_pushstring(L, instead_exec);
	free(instead_exec);
	return 1;
}

static int luaB_get_gamepath(lua_State* L) {
	char* game_path = getGamePath();
	strcat(game_path, "\\");
	//unix_path(game_path);
	lua_pushstring(L, game_path);
	free(game_path);
	return 1;
}

static int luaB_get_gamespath(lua_State *L) {
	char* games_path =getGamePath();
	strcat(games_path,"..");
	//unix_path(games_path);
	lua_pushstring(L, games_path);
	free(games_path);
	return 1;
}

const luaL_Reg paths_funcs[] = {
	{"instead_savepath", luaB_get_savepath},
	{"instead_exepath", luaB_get_exepath},
	{"instead_gamepath",luaB_get_gamepath},
	{"instead_gamespath", luaB_get_gamespath},
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
