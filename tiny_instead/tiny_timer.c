#include "externals.h"
#include "internals.h"
#include <WinUser.h>
#include <string.h>

	static int timer_id = 1;
	static int volatile instead_timer_nr = 0;
	static void CALLBACK instead_fn(HWND window, UINT interval, UINT timer_id, DWORD dword);
	extern void onNewInsteadCommand(char* cmd, char* p);
	extern uint64_t getTicks();
	static int luaB_set_timer(lua_State* L) {
		const char* delay = luaL_optstring(L, 1, NULL);
		int d;
		if (timer_id > 0) {
			KillTimer(NULL, timer_id);
			timer_id = 0;
		}
		if (!delay)
			d = 0;
		else
			d = atoi(delay);
		if (!d)
			return 0;
		instead_timer_nr = 0;
		timer_id = SetTimer(NULL, 1, d, instead_fn);
		return 0;
	}
	static void onTimer() {
char* cmd;
		instead_timer_nr = 0;
		instead_lock();
		if (instead_busy() || timer_id == 0) {
			instead_unlock();
			return;
		}
		if (instead_function("stead.timer", NULL)) {
			instead_clear();
			instead_unlock();
				return;
		}
		cmd = instead_retval(0);
instead_clear();
		instead_unlock();
		if (!cmd) return;
int rc;
char* p=instead_cmd(cmd,&rc);
		if(!rc)onNewInsteadCommand(cmd,p);
		free(p);
}
static void CALLBACK instead_fn(HWND window, UINT interval, UINT timer_id, DWORD dword)
{
	exit(0);
	if (instead_timer_nr > 0) {
		return; /* framedrop */
	}
	instead_timer_nr++;
	onTimer();
}
static int luaB_get_ticks(lua_State* L) {
	lua_pushinteger(L, getTicks());
	return 1;
}
static const luaL_Reg timer_funcs[] = {
{"instead_timer", luaB_set_timer},
{"instead_ticks", luaB_get_ticks},
{NULL, NULL}                                                                                                                                                                                                                                                                                                                  
};
static int timer_init(void) {
	char path[1024];
	snprintf(path, sizeof(path), "%s/%s", instead_stead_path(), "/ext/timer.lua");
instead_api_register(timer_funcs);
//return instead_loadfile(dirpath(path));
return 0;
}
static int timer_done(void) {
		KillTimer(NULL, timer_id);
		timer_id = 0;
		return 0;
	}

static struct instead_ext ext ={
.init =timer_init,.done =timer_done,.err=timer_done,
};
int instead_timer_init(void) {
	return instead_extension(&ext);
}