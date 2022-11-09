#include "externals.h"
#include "internals.h"

#include "bass.h"
extern void playSound(int isLooping);
int game_change_vol(int d, int val)
{

}

int gBassInit = 0;

static char play_mus[1024];
static HSAMPLE back_music;
static HCHANNEL back_channel;
static HMUSIC back_mod;
static float global_snd_lvl = 1.0f;
static int luaB_volume_sound(lua_State* L) {
	int rc;
	const char* fname = luaL_optstring(L, 1, NULL);
return global_snd_lvl;
}

static int luaB_free_sound(lua_State *L) {
//sound_done();
	return 1;
}

static int luaB_free_sounds(lua_State *L) {
//sound_done();
	return 1;
}

static int luaB_load_sound(lua_State *L) {
return 0;
}
static const luaL_Reg sound_funcs[] = {
		{"instead_sound_volume", luaB_volume_sound},
			{"instead_sound_free",luaB_free_sound},
			{"instead_sounds_free",luaB_free_sounds},
	{"instead_sound_load", luaB_load_sound},
	{NULL, NULL}
};
static int sound_done(void)
{
	strcpy(play_mus, "");
	return 0;
}
static int sound_init(void)
{
	int rc;
	instead_api_register(sound_funcs);
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
	char *mus;
	//Не инициализирован BASS
	if (!gBassInit) return;
	//Если отключено или нет звуков
	//if (!snd_volume_mus(-1))
	//	return;
	//if (!opt_music || !curgame_dir)
	//	return;
	//Получаем музыку
	instead_lock();
	instead_function("instead.get_music", NULL);
	mus = instead_retval(0);
	loop = instead_iretval(1);
	unix_path(mus);
	instead_clear();
	/* это не надо пока - настройка усиления/уменьшения
	instead_function("instead.get_music_fading", NULL);
	cf_out = instead_iretval(0);
	cf_in = instead_iretval(1);
	instead_clear();
	instead_unlock();
	*/

	if (mus && loop == -1) { /* disabled, 0 - forever, 1-n - loops */
		BASS_ChannelStop(back_channel);
		strcpy(play_mus, "");
		free(mus);
		mus = NULL;
	}

	if (loop == 0)
		loop = -1;

	//Играем музыку, надо учитывать останов предыдущей
	if (mus) {
		if (strcmp(play_mus, mus) != 0)
		{
			if (back_channel) BASS_ChannelStop(back_channel);
			strcpy(play_mus, mus);

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
					back_channel = BASS_SampleGetChannel(back_music, FALSE);
				}
			}
			
			if (back_channel) {
				BASS_ChannelSetAttribute(back_channel, BASS_ATTRIB_VOL, global_snd_lvl);
				BASS_ChannelPlay(back_channel, FALSE);
			}
		}
		
		//Выдача ошибки, если что не так
		//game_res_err_msg(mus, debug_sw);
		free(mus);
	}
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
