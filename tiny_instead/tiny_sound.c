#include "externals.h"
#include "internals.h"

#include "bass.h"
static int callback_ref =0;
#define SND_FMT_STEREO	1
#define SND_FMT_44	2
#define SND_FMT_22	4
#define SND_FMT_11	8

#define SND_CHANNELS 8
#define MAX_WAVS SND_CHANNELS * 2
#define SND_F2S(v) ((short)((float)(v) * 16383.0) * 2)
#define SOUND_MAGIC 0x2004
struct lua_sound {
	int type;
	short* buf;
	int len;
};
extern void playSound(int isLooping);
int snd_vol_from_pcn(int v)
{
	return (v * 127) / 100;
}
static int snd_vol_to_pcn(int v)
{
	return (v * 100) / 127;
}
int game_change_vol(int d, int val)
{

}

int gBassInit = 0;

static char* play_mus =NULL;
static HSAMPLE back_music;
static HCHANNEL back_channel;
static float global_snd_lvl = 1.0f;
static void musFree() {
	if (back_channel) {
		BASS_ChannelFree(back_channel) ||BASS_MusicFree(back_channel) || BASS_StreamFree(back_channel);
		back_channel = 0;
	}
	if (back_music) {
		BASS_SampleFree(back_music);
		back_music = 0;
}
	if (play_mus) {
		free(play_mus);
		play_mus = NULL;
	}
}
static void sounds_free() {
	}
static void CALLBACK finishCallback(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	instead_lock();
	instead_function("instead.finish_music", NULL);
	//int rc = instead_bretval(0);
	instead_clear();
	instead_unlock();
	musFree();
}

static int luaB_is_sound(lua_State* L) {
	return 0;
}
static int luaB_volume_sound(lua_State* L) {
	int vol = luaL_optnumber(L, 1, -1);
	//vol = snd_volume_mus(vol);
	lua_pushinteger(L, vol);
	return 0;
}
static int luaB_channel_sound(lua_State* L) {
	const char* s;
	int ch = luaL_optinteger(L, 1, 0);
	return 0;
}
static int luaB_panning_sound(lua_State* L) {
	int chan = luaL_optinteger(L, 1, -1);
	int left = luaL_optnumber(L, 2, 255);
	int right = luaL_optnumber(L, 3, 255);
	return 0;
}

static int luaB_free_sound(lua_State *L) {
	const char* fname = luaL_optstring(L, 1, NULL);
	if (!fname)
		return 0;
	//sound_unload(fname);
	return 0;
	}

static int luaB_free_sounds(lua_State *L) {
	sounds_free();
	return 0;
}

static int luaB_load_sound(lua_State *L) {
	const char* fname = luaL_optstring(L, 1, NULL);
return 0;
}

static int luaB_load_sound_mem(lua_State* L) {
	int hz = luaL_optinteger(L, 1, -1);
	int channels = luaL_optinteger(L, 2, -1);
	int len; int i;
	short* buf = NULL;
	const char* name;
	int fmt = 0;
	luaL_checktype(L, 3, LUA_TTABLE);
		if (hz < 0 || channels < 0)
		return 0;
#if LUA_VERSION_NUM >= 502
	len = lua_rawlen(L, 3);
#else
	len = lua_objlen(L, 3);
#endif
	if (len <= 0)
		return 0;
	buf = malloc(sizeof(short) * len);
	if (!buf)
		return 0;

	lua_pushvalue(L, 3);

	for (i = 0; i < len; i++) {
		float v;
		lua_pushinteger(L, i + 1);
		lua_gettable(L, -2);

		if (!lua_isnumber(L, -1)) {
			v = 0;
		}
		else {
			v = (float)lua_tonumber(L, -1);
		}
		buf[i] = SND_F2S(v);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	/* here we got the sample */
	if (channels == 2)
		fmt |= SND_FMT_STEREO;

	if (hz == 11025)
		fmt |= SND_FMT_11;
	else if (hz == 22050)
		fmt |= SND_FMT_22;
	else
		fmt |= SND_FMT_44;
	//name = sound_load_mem(fmt, buf, len);
	name = "";
	/*	free(buf); */
	if (!name)
		return 0;
	lua_pushstring(L, name);
}
static int luaB_music_callback(lua_State* L) {
	if (!gBassInit)
		return 0;
	//snd_mus_callback(NULL, NULL);
	if (callback_ref) {
		luaL_unref(L, LUA_REGISTRYINDEX, callback_ref);
		callback_ref = 0;
	}
		if (lua_isfunction(L, 1)) {
			musFree(play_mus);
		callback_ref = luaL_ref(L, LUA_REGISTRYINDEX);
		//snd_mus_callback(mus_callback, L);
	}
	return 0;
}
static const luaL_Reg sound_funcs[] = {
	{
		"instead_sound", luaB_is_sound},
		{"instead_sound_volume", luaB_volume_sound},
			{"instead_sound_free",luaB_free_sound},
			{"instead_sounds_free",luaB_free_sounds},
	{"instead_sound_load", luaB_load_sound},
	{"instead_sound_load_mem", luaB_load_sound_mem},
		{"instead_sound_channel", luaB_channel_sound},
	{"instead_sound_panning", luaB_panning_sound},
	{"instead_music_callback", luaB_music_callback},
	{NULL, NULL}
};
static int sound_value(lua_State* L) {
	struct lua_sound* hdr = (struct lua_sound*)lua_touserdata(L, 1);
	int pos = luaL_optinteger(L, 2, -1);
	float v = luaL_optnumber(L, 3, 0.0f);
	if (pos <= 0)
		return 0;
	if (pos > hdr->len)
		return 0;
	pos--;
	if (lua_isnoneornil(L, 3)) {
		lua_pushinteger(L, hdr->buf[pos]);
		return 1;
	}
	hdr->buf[pos] = SND_F2S(v);
	return 0;
}

static int chunk_create_meta(lua_State* L) {
	luaL_newmetatable(L, "soundbuffer metatable");
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, sound_value);
	lua_settable(L, -3);

	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, sound_value);
	lua_settable(L, -3);
	/*
		lua_pushstring (L, "size");
		lua_pushcfunction (L, sound_size);
		lua_settable(L, -3);
	*/
	return 0;
}
static int sound_done(void)
{
	if (!gBassInit) return 0;
	if (callback_ref) {
		//snd_mus_callback(NULL, NULL);
		luaL_unref(instead_lua(), LUA_REGISTRYINDEX, callback_ref);
		callback_ref = 0;
	}
	musFree();
	sounds_free();
	return 0;
}

static int sound_init(void)
{
	int rc;

	instead_api_register(sound_funcs);
	chunk_create_meta(instead_lua());
	/*		char path[PATH_MAX];
	snprintf(path, sizeof(path), "%s/%s", instead_stead_path(), "/ext/sound.lua");
	rc = instead_loadfile(dirpath(path));
	if (rc)
		return rc;*/
return 0;
}

static const char *get_filename_ext(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";
	return dot + 1;
}

//Проигрывание музыки
static void game_music_player(void)
{
	int	loop;
	//Не инициализирован BASS
	if (!gBassInit) return;
	//Если отключено или нет звуков
	//if (!snd_volume_mus(-1))
	//	return;
	//if (!opt_music || !curgame_dir)
	//	return;
		//Получаем музыку
	char* mus;
	instead_lock();
	instead_function("instead.get_music", NULL);
	mus = instead_retval(0);
	loop = instead_iretval(1);
	unix_path(mus);
	instead_clear();
instead_function("instead.get_music_fading", NULL);
	int cf_out = instead_iretval(0);
	int cf_in = instead_iretval(1);
	instead_clear();
	instead_unlock();
		if (mus && loop == -1) { /* -1 - disabled, 0 - forever, 1-n - loops */
						free(mus);
			mus = NULL;
	}
	if (loop == 0)
		loop = -1;
	if (cf_out == 0)
		cf_out = 500;
	else if (cf_out < 0)
		cf_out = 0;

	if (cf_in < 0)
		cf_in = 0;
	//Играем или останавливаем музыку, надо учитывать останов предыдущей
if(!mus &&play_mus) {
			//Останавливаем музыку
		musFree();
}
else if (mus &&(!play_mus || strcmp(play_mus, mus)))
		{
musFree();
			play_mus = mus;
						DWORD loop_flag = 0;
			if (loop <= 0) loop_flag |= BASS_SAMPLE_LOOP;
			if ((strcmp(get_filename_ext(mus),"xm")==0) ||
				(strcmp(get_filename_ext(mus),"s3m")==0) ||
				(strcmp(get_filename_ext(mus),"mod")==0) ||
				(strcmp(get_filename_ext(mus), "mo3") == 0) ||
				(strcmp(get_filename_ext(mus), "it") == 0) ||
				(strcmp(get_filename_ext(mus), "mtm") == 0) ||
				(strcmp(get_filename_ext(mus), "umx") == 0)
				)
			{
				back_channel = BASS_MusicLoad(FALSE, mus, 0, 0, loop_flag, 0);
			}
			else
			{
				back_music = BASS_SampleLoad(FALSE, mus, 0, 0, 1, loop_flag);
				if (back_music) {
					back_channel = BASS_SampleGetChannel(back_music, BASS_SAMCHAN_STREAM);
				}
			}
			if (back_channel) {
								BASS_ChannelSetAttribute(back_channel, BASS_ATTRIB_VOL, global_snd_lvl);
								//Отслеживаем завершение воспроизведения.
								if (!loop_flag) BASS_ChannelSetSync(back_channel, BASS_SYNC_ONETIME| BASS_SYNC_END, 0, finishCallback, 0);
BASS_ChannelPlay(back_channel, FALSE);
			}
		}
		//Выдача ошибки, если что не так
		//game_res_err_msg(mus, debug_sw);
}

//Проигрывание звуков
static void game_sound_player(void)
{
	char		*snd;
	int		chan = -1;
	int		loop = 1;

	struct instead_args args[] = {
		{ .val = "nil",.type = INSTEAD_NIL },
		{ .val = "-1",.type = INSTEAD_NUM },
		{ .val = NULL }
	};

	//Не инициализирован BASS
	if (!gBassInit) return;
	//Если отключено
	//if (!snd_volume_mus(-1))
	//	return;

	instead_lock();
	instead_function("instead.get_sound", NULL);

	loop = instead_iretval(2);
	chan = instead_iretval(1);
	snd = instead_retval(0);
	instead_clear();
	if (!snd) {
		if (chan != -1) {
			/* halt channel */
			//snd_halt_chan(chan, 500);
			instead_function("instead.set_sound", args); 
			instead_clear();
		}
		instead_unlock();
		return;
	}
	instead_function("instead.set_sound", args); instead_clear();
	instead_unlock();
	unix_path(snd);
	//Проигрывание звука - пока недоступно
	//_play_combined_snd(snd, chan, loop);
	playSound(snd,loop);
	free(snd);
}

static int sound_cmd(void)
{
	game_music_player();
	game_sound_player();
	return 0;
}

static struct instead_ext ext = {
	.init = sound_init,
	.done = sound_done,
	.cmd = sound_cmd,
};

int instead_sound_init(void)
{
	return instead_extension(&ext);
}

void stopAllSound()
{
	if (back_channel) BASS_ChannelStop(back_channel);
}

//Установить уровень звука для музыки+звуков
void setGlobalSoundLevel(int volume)
{
	int i;
	if (volume<0) volume = 0;
	if (volume>100) volume = 100;
	//Для всех работающих каналов
	global_snd_lvl = volume / 100.0f;
	//  музыка
	if (back_channel) BASS_ChannelSetAttribute(back_channel, BASS_ATTRIB_VOL, global_snd_lvl);
}

int getGlobalSoundLevel()
{
	return global_snd_lvl * 100;
}
