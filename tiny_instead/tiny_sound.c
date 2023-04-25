#include "externals.h"
#include "internals.h"
#include "bass.h"
static int callback_ref = 0;
#define SND_FMT_STEREO	1
#define SND_FMT_44	2
#define SND_FMT_22	4
#define SND_FMT_11	8

#define SND_CHANNELS 16
#define MAX_WAVS SND_CHANNELS * 2
#define SND_F2S(v) ((short)((float)(v) * 16383.0) * 2)
#define SOUND_MAGIC 0x2004
struct lua_sound {
	int type;
	short* buf;
	int len;
};
static LIST_HEAD(sounds);
static int sounds_nr = 0;
static	char* last_music = NULL;
static HSAMPLE back_music;
static HCHANNEL back_channel;
static HSAMPLE old_music; //Не ноль,если используется приглушение,и оно ещё не завершилось.
static HCHANNEL old_channel;
static float global_mus_lvl = 1.0f, global_sounds_lvl = 1.0f;
static int mus_loop;
typedef struct {
	struct list_node list;
	char* fname;
	HSAMPLE sam;
	HCHANNEL channels[SND_CHANNELS]; //Чтобы один звук мог воспроизводиться в нескольких каналах (при необходимости).
	int	loaded;
	int	system;
	int	fmt;
	short* buf;
	size_t	len;
} _snd_t;
typedef struct {
	_snd_t* snd;
	int	loop,index;
	float vol,pan;
	HSYNC sync;
} _snd_chan_t;
typedef struct {
	_snd_t* snd;
	int	loop;
	int	channel;
} _snd_req_t;
static _snd_chan_t channels[SND_CHANNELS];
static _snd_req_t sound_reqs[SND_CHANNELS];
static void CALLBACK finishCallback(HSYNC handle, DWORD channel, DWORD data, void* user);
static void fun(int a);
static void mus_callback(void* udata, unsigned char* stream, int len)
{
	lua_State* L = (lua_State*)udata;
	struct lua_sound* hdr;
	if (!callback_ref)
		return;
	instead_lock();
	lua_rawgeti(L, LUA_REGISTRYINDEX, callback_ref);
	lua_pushinteger(L, snd_hz());
	lua_pushinteger(L, len >> 1);
	hdr = lua_newuserdata(L, sizeof(*hdr));
	if (!hdr)
		goto err;
	hdr->type = SOUND_MAGIC;
	hdr->len = len >> 1; /* 16bits */
	hdr->buf = (short*)stream;
	luaL_getmetatable(L, "soundbuffer metatable");
	lua_setmetatable(L, -2);
	if (instead_pcall(L, 3)) { /* on any error */
	err:
		//snd_mus_callback(NULL, NULL);
		luaL_unref(L, LUA_REGISTRYINDEX, callback_ref);
		callback_ref = 0;
	}
	instead_clear();
	instead_unlock();
	return;
}
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

static int  sound_playing(_snd_t* snd)
{
	int i;
	for (i = 0; i < SND_CHANNELS; i++) {
		if (&channels[i] &&channels[i].snd == snd)
			return i;
		if (&sound_reqs[i] &&sound_reqs[i].snd == snd)
			return i;
	}
	return -1;
}
static const char* sound_channel(int i)
{
	_snd_t* sn;
	if (i >= SND_CHANNELS)
		i = i % SND_CHANNELS;
	if (i == -1) {
		for (i = 0; i < SND_CHANNELS; i++) {
			sn = channels[i].snd;
			if (sn && !sn->system)
				return sn->fname;
		}
		return NULL;
	}
	sn = channels[i].snd;
	if (!sn || sn->system)
		return NULL; /* NULL or hidden system sound */
	return sn->fname;
}
static void media_free(HSAMPLE* sam, HCHANNEL* chan) {
		BOOL result = FALSE;
	if (*chan) {
		result = BASS_ChannelFree(*chan) || BASS_MusicFree(*chan) || BASS_StreamFree(*chan);
		*chan = 0;
	}
	if (*sam) {
		result |= BASS_SampleFree(*sam);
		*sam = 0;
	}
}
static void sound_free(_snd_t* sn,boolean removeFromList)
{
	if (!sn)
		return;
	if (removeFromList) {
		list_del(&sn->list);
		sounds_nr--;
	}
	if (sn->fname) {
		free(sn->fname);
		sn->fname = NULL;
	}
	if (sn->buf) {
		free(sn->buf);
		sn->buf = NULL;
	}
	for (int a = 0; a < SND_CHANNELS;a++)media_free(&sn->sam, &sn->channels[a]);
	free(sn);
	sn = NULL;
}
/*static void sound_free(_snd_t* sn) {
	sound_free(sn, TRUE);
}*/
static void sounds_free(void)
{
	int i = 0;
	_snd_t* pos, * pos2;
	_snd_t* sn;
	pos = list_top(&sounds, _snd_t, list);
	while (pos) {
		sn = (_snd_t*)pos;
		pos2 = list_next(&sounds, pos, list);
		if (sn->system)
			sn->loaded = 1; /* ref by system only */
		else
			sound_free(sn,TRUE);
		pos = pos2;
	}
	for (i = 0; i < SND_CHANNELS; i++) {
		channels[i].snd = NULL;
		channels[i].sync = 0;
		sound_reqs[i].snd = NULL;
	}
	/*	sounds_nr = 0;
		fprintf(stderr, "%d\n", sounds_nr); */
		//input_uevents(); /* all callbacks */
}
static _snd_t* sound_find(const char* fname)
{
	_snd_t* pos = NULL;
	_snd_t* sn;
	if (!fname)
		return NULL;
	list_for_each(&sounds, pos, list) {
		sn = (_snd_t*)pos;
		if (!strcmp(fname, sn->fname)) {
			list_del(&sn->list);
			list_add(&sounds, &sn->list); /* move it on head */
			return sn;
		}
	}
	return NULL;
}

static int sound_find_channel(void)
{
	int i;
	for (i = 0; i < SND_CHANNELS; i++) {
		if (!channels[i].snd && !sound_reqs[i].snd)
			return i;
	}
	return -1;
}
static void sounds_shrink(void)
{
	_snd_t* pos, * pos2;
	_snd_t* sn;
	pos = list_top(&sounds, _snd_t, list);
	/*	fprintf(stderr,"shrink try\n"); */
	while (pos && sounds_nr > MAX_WAVS) {
		sn = (_snd_t*)pos;
		if (sound_playing(sn) != -1 || sn->loaded) {
			pos = list_next(&sounds, pos, list);
			continue;
		}
		pos2 = list_next(&sounds, pos, list);
		sound_free(sn,TRUE);
		pos = pos2;
		/*		fprintf(stderr,"shrink by 1\n"); */
	}
}

static void musFree(int cf_out) {
	if (old_music || !cf_out) //Освобождаем канал,если нет приглушения,или если уже приглушение не завершено и old_music присвоено значение приглушаемой музыке,а back_channel уже получил следующую музыку,к примеру при выполнении команды перед этой командой.
		media_free(&back_music, &back_channel);
	if (last_music) {
		free(last_music);
		last_music = NULL;
		mus_loop = 0;
	}
}
static void halt_chan(int chan, int ms)
{
	if (chan >= SND_CHANNELS)chan %= SND_CHANNELS;
	if (chan == -1) {
		for (int a = 0; a < SND_CHANNELS; a++) halt_chan(a, ms);
		return;
	}
	if (!channels[chan].snd) return;
	channels[chan].loop = 1;
		if (BASS_ChannelFlags(channels[chan].snd->channels[chan], 0, 0) & BASS_SAMPLE_LOOP) BASS_ChannelFlags(channels[chan].snd->channels[chan], 0, BASS_SAMPLE_LOOP);
		BASS_ChannelRemoveSync(channels[chan].snd->channels[chan], channels[chan].sync);
		channels[chan].sync = 0;
		if (!ms) {
			BASS_ChannelStop(channels[chan].snd->channels[chan]);
			fun(chan);
		}
		else {
			BASS_ChannelSetSync(channels[chan].snd->channels[chan], BASS_SYNC_SLIDE, 0, finishCallback, &channels[chan]);
			BASS_ChannelSlideAttribute(channels[chan].snd->channels[chan], BASS_ATTRIB_VOL, -1.0f, ms);
		}
}
static const char* get_filename_ext(const char* filename) {
	const char* dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";
	return dot + 1;
}
//Получение канала и hsample
static void setPlayableInfo(char* mus, HSAMPLE* sam, HCHANNEL* channel, DWORD loop_flag, int cf_in) {
	if (!*channel &&(strcmp(get_filename_ext(mus), "xm") == 0 ||
		strcmp(get_filename_ext(mus), "s3m") == 0 ||
		strcmp(get_filename_ext(mus), "mod") == 0 ||
		strcmp(get_filename_ext(mus), "mo3") == 0 ||
		strcmp(get_filename_ext(mus), "it") == 0 ||
		strcmp(get_filename_ext(mus), "mtm") == 0 ||
		strcmp(get_filename_ext(mus), "umx") == 0)
			)
{
		*channel = BASS_MusicLoad(FALSE, mus, 0, 0, loop_flag | BASS_SAMPLE_FLOAT | BASS_MUSIC_SINCINTER | BASS_MUSIC_FT2MOD | BASS_MUSIC_FT2PAN | BASS_MUSIC_RAMP, 0);
	}
	else if(!*sam)
	{
		*sam = BASS_SampleLoad(FALSE, mus, 0, 0, 1, loop_flag | BASS_SAMPLE_FLOAT);
	}
	if (*sam &&!*channel) {
		/**channel = BASS_SampleGetChannel(*sam, BASS_SAMCHAN_STREAM|BASS_STREAM_DECODE);
		float level;
		BOOL success = BASS_ChannelGetLevelEx(*channel, &level, 10000, BASS_LEVEL_MONO);
		char buf[32];
		snprintf(buf, sizeof(buf), "%f", level);
		MessageBoxA(0, buf, "", 0);*/
		*channel = BASS_SampleGetChannel(*sam, BASS_SAMCHAN_STREAM|BASS_SAMCHAN_NEW);
	}
	if (*channel) {
		BASS_ChannelSetAttribute(*channel, BASS_ATTRIB_VOL, cf_in ? 0.0f : *channel == back_channel ? global_mus_lvl : global_sounds_lvl);
		if (cf_in) BASS_ChannelSlideAttribute(*channel, BASS_ATTRIB_VOL, *channel == back_channel ? global_mus_lvl: global_sounds_lvl, cf_in);
			}
}
//Добавление звука к списку звуков.
static _snd_t* sound_add(const char* fname, int fmt, short* buf, int len)
{
	_snd_t* sn;
	if (!fname || !*fname || !gBassInit) return NULL;
	sn = malloc(sizeof(_snd_t));
	if (!sn)
		return NULL;
	memset(sn, 0, sizeof(*sn));
	/*	LIST_HEAD_INIT(&sn->list); */
	sn->fname = strdup(fname);
	sn->loaded = 0;
	sn->system = 0;
	sn->buf = buf;
	sn->len = len;
	sn->fmt = fmt;
	if (!sn->fname) {
		sound_free(sn,TRUE);
		return NULL;
	}
	if (buf) {
		//w =snd_load_mem(fmt, buf, len);
	}
	else {
		for (int a = 0; a < SND_CHANNELS; a++) {
			setPlayableInfo(sn->fname, &sn->sam, &sn->channels[a], 0, 0);
			if (!sn->channels[a]) { //Возникла ошибка
				sound_free(sn,FALSE);
				return NULL;
			}
		}
}
		//if (!sn->channels[0]) goto err;
	sounds_shrink();
	list_add(&sounds, &sn->list);
	sounds_nr++;
	return sn;
/*err:
	free(sn);
	sn = NULL;
	return NULL;*/
}
static void sound_play(_snd_t* sn, int chan, int loop) {
	int c;
	if (!sn)
		return;
	if (chan == -1) {
		c = sound_find_channel();
		if (c == -1)
			return; /* all channels are busy */
	}
	else
		c = chan;
	if (channels[c].snd) {
				sound_reqs[c].snd = sn;
		sound_reqs[c].loop = loop;
		sound_reqs[c].channel = chan;
			halt_chan(chan, 0); /* work in callback */
					//input_uevents(); /* all callbacks */
			return;
		}
	channels[c].snd = sn;
	channels[c].loop = loop;
	if (loop != 1 && !(BASS_ChannelFlags(sn->channels[c], 0, 0) & BASS_SAMPLE_LOOP))  BASS_ChannelFlags(sn->channels[c], BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
	BASS_ChannelSetAttribute(sn->channels[c], BASS_ATTRIB_PAN, channels[c].pan);
	//Удаляем callback,поскольку,к примеру,перед этим звук мог воспроизводиться в другом канале,а значит при срабатывании вызова мы будем работать с другим звуком.
	if (channels[c].sync) BASS_ChannelRemoveSync(channels[c].snd->channels[c], channels[c].sync);
	channels[c].index = c;
	channels[c].sync = BASS_ChannelSetSync(sn->channels[c], /*BASS_SYNC_ONETIME |*/ BASS_SYNC_MIXTIME|BASS_SYNC_END, 0, finishCallback, &channels[c]);
	BASS_ChannelPlay(sn->channels[c], FALSE);
		/*	fprintf(stderr, "added: %d\n", c); */
}
static int _play_combined_snd(char* filename, int chan, int loop)
{
	char* str;
	char* p, * ep;
	_snd_t* sn;
	p = str = strdup(filename);
	if (!str)
		return -1;
	p = strip(p);
	while (*p) {
		int c = chan, l = loop;
		int at = 0;
		ep = p + strcspn(p, ";@");
		if (*ep == '@') {
			at = 1;
			*ep = 0; ep++;
			if (sscanf(ep, "%d,%d", &c, &l) > 1)
				at++;
			ep += strcspn(ep, ";");
			if (*ep)
				ep++;
		}
		else if (*ep == ';') {
			*ep = 0; ep++;
		}
		else if (*ep) {
			goto err;
		}
		p = strip(p);
		sn = sound_find(p);
		if (!sn)
			sn = sound_add(p, 0, NULL, 0);
		if (sn)
			sound_play(sn, c, l);
		else if (at || c != -1) { /* if @ or specific channel */
halt_chan(c, (at == 2) ? l : 0);
			/*char buf[32];
			sprintf(buf, "%d", c);
			MessageBox(0, buf, "", 0);*/
		}
		p = ep;
	}
	free(str);
	return 0;
err:
	free(str);
	return -1;
}
static void fun(int a) {
_snd_req_t* r = &sound_reqs[a];
BASS_ChannelSetAttribute(channels[a].snd->channels[a], BASS_ATTRIB_VOL, global_sounds_lvl);
channels[a].snd = NULL;
	if (r->snd) {
		_snd_t* s = r->snd;
		r->snd = NULL;
		sound_play(s, a, r->loop);
	}
	else {
		halt_chan(a, 0); // to avoid races
	}
}
static void finishMusicLua() {
	instead_lock();
	instead_function("instead.finish_music", NULL);
	int rc = instead_bretval(0);
	instead_clear();
	instead_unlock();
}
//Некоторые трекерные файлы,в основном вроде .xm и .mod файлы,имеют команду F00,что приводит к остановке воспроизведения,т.к bass так её воспринимает (см https://www.un4seen.com/forum/?topic=20037.msg140533#msg140533).
static boolean playingIsContinue(DWORD channel) {
	return BASS_ChannelSetPosition(channel, BASS_ChannelGetPosition(channel, BASS_POS_MUSIC_ORDER | BASS_POS_DECODE) + 1, BASS_POS_MUSIC_ORDER);
}
static void CALLBACK finishCallback(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	if (channel == old_channel ) {
		//Приглушение завершено.
		media_free(&old_music, &old_channel);
			BASS_ChannelSetAttribute(back_channel, BASS_ATTRIB_VOL, global_mus_lvl);
			if(back_channel)BASS_ChannelPlay(back_channel, FALSE);
	}
	else if (channel == back_channel) {
		if (playingIsContinue(back_channel)) return;
		if (mus_loop != 1) {
			if (mus_loop > 1) mus_loop--;
			return;
		}
		//Канал остановлен,или приглушение завершено и больше нечего играть.
		finishMusicLua();
	}
	else {
		/*for (int a = 0; a < SND_CHANNELS; a++) {
			if (channels[a].snd && channel == channels[a].snd->channels[a]) {
if(playingIsContinue(channels[a].snd->channels[a])) return;
				if (channels[a].loop != 1) {
					if (channels[a].loop > 1) channels[a].loop--;
					return;
				}
				//Приглушение
							if (handle != channels[a].sync) {
BASS_ChannelRemoveSync(channels[a].snd->channels[a], handle);
BASS_ChannelSetAttribute(channels[a].snd->channels[a], BASS_ATTRIB_VOL, global_sounds_lvl);
}
halt_chan(a,0);
									break;
			}*/
		if (!user) return;
		int a = (* (_snd_chan_t*)user).index;
				if (channels[a].snd && channel == channels[a].snd->channels[a]) {
					if (playingIsContinue(channels[a].snd->channels[a])) return;
			if (channels[a].loop != 1) {
				if (channels[a].loop > 1) channels[a].loop--;
				return;
			}
			//Приглушение
			if (handle != channels[a].sync) {
				BASS_ChannelRemoveSync(channels[a].snd->channels[a], handle);
BASS_ChannelSetAttribute(channels[a].snd->channels[a], BASS_ATTRIB_VOL, global_sounds_lvl);
}
			halt_chan(a, 0);
		}
			}
}

static void sounds_reload(void)
{
	_snd_t* pos = NULL;
	_snd_t* sn;
	int i;
	halt_chan(-1, 0); /* stop all sound */
	list_for_each(&sounds, pos, list) {
		sn = (_snd_t*)pos;
		for (int a = 0; a < SND_CHANNELS; a++) {
			media_free(&sn->sam, &sn->channels[a]);
			if(!sn->buf)setPlayableInfo(sn->fname, &sn->sam, &sn->channels[a], 0, 0);
		}
		if (sn->buf)
			/*sn->wav =*/ snd_load_mem(sn->fmt, sn->buf, sn->len);
	}
	for (i = 0; i < SND_CHANNELS; i++) {
		channels[i].snd = NULL;
		sound_reqs[i].snd = NULL;
	}
	//input_uevents(); /* all callbacks */
}
static void* _sound_get(const char* fname, int fmt, short* buff, size_t len)
{
	_snd_t* sn = NULL;
	if (fname) {
		sn = sound_find(fname);
		if (sn) {
			sn->loaded++; /* to pin */
			return sn;
		}
		sn = sound_add(fname, fmt, buff, len);
	}
	else if (buff) {
		char* name = malloc(64);
		if (!name)
			return NULL;
		snprintf(name, 64, "snd:%p", buff); name[64 - 1] = 0;
		sn = sound_add(name, fmt, buff, len);
		if (!sn)
			free(name);
		else {
			snprintf(name, 64, "snd:%p", sn); name[64 - 1] = 0;
			free(sn->fname);
			sn->fname = name;
		}
	}
	if (!sn)
		return NULL;
	sn->loaded = 1;
	return sn;
}

static void _sound_put(void* s)
{
	_snd_t* sn = (_snd_t*)s;
	if (!sn || !sn->loaded)
		return;
	if (!sn->system || sn->loaded > 1)
		sn->loaded--;
	if (!sn->loaded && sound_playing(sn) == -1)
		sound_free(sn,TRUE);
	return;
}

void* sound_get(const char* fname)
{
	_snd_t* sn = _sound_get(fname, 0, NULL, 0);
	if (!sn)
		return NULL;
	//sn->system = 1;
	return sn;
}

void sound_put(void* s)
{
	_snd_t* sn = (_snd_t*)s;
	if (!sn)
		return;
	sn->system = 0;
	_sound_put(sn);
}
static int sound_load(const char* fname)
{
	_snd_t* sn = _sound_get(fname, 0, NULL, 0);
	if (!sn)
		return -1;
	return 0;
}
static char* sound_load_mem(int fmt, short* buff, size_t len)
{
	_snd_t* sn = _sound_get(NULL, fmt, buff, len);
	if (!sn)
		return NULL;
	return sn->fname;
}
static void sound_unload(const char* fname)
{
	_snd_t* sn;
	sn = sound_find(fname);
	_sound_put(sn);
	return;
}
static int luaB_is_sound(lua_State* L) {
	int chan = luaL_optinteger(L, 1, -1);
	lua_pushboolean(L, (sound_channel(chan) != NULL));
	return 1;
}
static int luaB_volume_sound(lua_State* L) {
	int vol = luaL_optnumber(L, 1, -1);
	//vol = snd_volume_mus(vol);
	lua_pushinteger(L, vol);
	return 1;
}
static int luaB_load_sound(lua_State* L) {
	int rc;
	const char* fname = luaL_optstring(L, 1, NULL);
	if (!fname)
		return 0;
	rc = sound_load(fname);
	if (rc)
		return 0;
	lua_pushstring(L, fname);
	return 1;
}
static int luaB_load_sound_mem(lua_State* L) {
	int hz = luaL_optinteger(L, 1, -1);
	int channels = luaL_optinteger(L, 2, -1);
	int len, i;
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
	name = sound_load_mem(fmt, buf, len);
	/*	free(buf); */
	if (!name)
		return 0;
	lua_pushstring(L, name);
	return 1;
}
static int luaB_channel_sound(lua_State* L) {
	const char* s;
	int ch = luaL_optinteger(L, 1, 0);
	ch = ch % SND_CHANNELS;
	s = sound_channel(ch);
	if (s) {
		lua_pushstring(L, s);
		return 1;
	}
	return 0;
}
static int luaB_panning_sound(lua_State* L) {
	int chan = luaL_optinteger(L, 1, -1);
	int left = luaL_optnumber(L, 2, 255);
	int right = luaL_optnumber(L, 3, 255);
	float vol, pan;
	if (left == right) {
		vol = left / 255.0f;
		pan = 0;
	}
	else if (left > right) {
		vol = left / 255.0f;
		pan = -1 + (right / left);
	}
	else {
		vol = right / 255.0f;
		pan = 1 - (left / right);
	}
	channels[chan].vol = vol;
	channels[chan].pan = pan;
	BASS_ChannelSetAttribute(channels[chan].snd->channels[chan], BASS_ATTRIB_PAN, pan);
	return 0;
}
static int luaB_free_sound(lua_State* L) {
	const char* fname = luaL_optstring(L, 1, NULL);
	if (!fname)
		return 0;
	sound_unload(fname);
	return 0;
}

static int luaB_free_sounds(lua_State* L) {
	sounds_free();
	return 0;
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
		musFree(0);
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
	media_free(&old_music, &old_channel);
	musFree(0);
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
//Проигрывание музыки
static void game_music_player(void)
{
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
	char* mus = instead_retval(0);
	int loop = instead_iretval(1);
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
	//Для библиотеки bass это не имеет значения
	/*if (loop == 0)
		loop = -1;*/
	/*if (cf_out == 0)
		cf_out = 500;
	else */ if (cf_out < 0)
		cf_out = 0;

	if (cf_in < 0)
		cf_in = 0;
#ifndef fad_music
#define fad_music if (cf_out && !old_music) {\
	/*Помечаем,что музыка ещё не остановлена.*/\
	if(back_music ||back_channel) {\
old_music = back_music;\
	old_channel = back_channel;\
back_music =0;\
back_channel =0;\
}\
BASS_ChannelSetSync(old_channel, BASS_SYNC_SLIDE, 0, finishCallback, 0);\
BASS_ChannelSlideAttribute(old_channel, BASS_ATTRIB_VOL, -1.0f, cf_out);\
}\
musFree(cf_out);
#endif
	//Играем или останавливаем музыку, надо учитывать останов предыдущей
	if (!mus && last_music) {
		//Останавливаем музыку
//game_stop_mus(cf_out);
		fad_music
	}
	else if (mus && (!last_music || strcmp(last_music, mus)))
	{
		//game_stop_mus(cf_out);
		fad_music
		last_music = mus;
		mus_loop = loop;
		DWORD loop_flag = 0;
		if (loop !=1) loop_flag |= BASS_SAMPLE_LOOP;
		setPlayableInfo(last_music, &back_music, &back_channel, loop_flag, cf_in);
		if (back_channel && !old_music) { //Играем только если канал создан и музыка полностью затухла
			//Не думаем об удалении вызовов,т.к мы используем музыку один раз,а потом полностью освобождаем её.
			BASS_ChannelSetSync(back_channel, /*BASS_SYNC_ONETIME |*/ BASS_SYNC_MIXTIME|BASS_SYNC_END, 0, finishCallback, 0);
			BASS_ChannelPlay(back_channel, FALSE);
			//free(mus);
		}
		else if (!back_channel) {
			//Выдача ошибки, если что не так
//game_res_err_msg(mus, debug_sw);
			if (mus) {
				free(mus);
				mus = NULL;
				last_music = NULL;
}
		}
	}
}

//Проигрывание звуков
static void game_sound_player(void)
{
	char* snd;
	int		chan = -1;
	int		loop = 1;

	struct instead_args args[] = {
	{.val = "nil",.type = INSTEAD_NIL },
	{.val = "-1",.type = INSTEAD_NUM },
	{.val = NULL }
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
	//if (chan==-1) MessageBoxA(0, "", buf, 0);
	if (!snd) {
		if (chan != -1) {
			/* halt channel */
			//halt_chan(chan, 500);
			halt_chan(chan, 0);
			instead_function("instead.set_sound", args);
			instead_clear();
		}
		instead_unlock();
		return;
	}
	instead_function("instead.set_sound", args);
	instead_clear();
	/*if (!instead_function("instead.get_sound_fading", NULL)) {
		int chan = instead_iretval(0);
		int cf_out = instead_iretval(1);
		int cf_in = instead_iretval(2);
		instead_clear();
			}*/
	instead_unlock();
	unix_path(snd);
	_play_combined_snd(snd, chan, loop);
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
//Установить уровень звука для музыки+звуков
void setGlobalMusicLevel(int volume)
{
	if (volume < 0) volume = 0;
	if (volume > 100) volume = 100;

	global_mus_lvl = volume / 100.0f;
	//  музыка
	if (back_channel)BASS_ChannelSetAttribute(back_channel, BASS_ATTRIB_VOL, global_mus_lvl);
}

int getGlobalMusicLevel()
{
	return global_mus_lvl * 100;
}
//Установить громкость для звуков игры.
void setGlobalSoundsLevel(int volume)
{
	if (volume < 0) volume = 0;
	if (volume > 100) volume = 100;
	//Для всех работающих каналов
	global_sounds_lvl = volume / 100.0f;
for (int a = 0; a < SND_CHANNELS; a++) {
		channels[a].vol = global_sounds_lvl;
		for (int b = 0; b < SND_CHANNELS; b++) {
			if (channels[a].snd && channels[a].snd->channels[b]) BASS_ChannelSetAttribute(channels[a].snd->channels[b], BASS_ATTRIB_VOL, global_sounds_lvl);
		}
	}
}

int getGlobalSoundsLevel()
{
	return global_sounds_lvl * 100;
}