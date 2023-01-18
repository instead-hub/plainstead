local std = stead

local instead = std.obj { nam = '@instead' }

instead.nosave = false
instead.noautosave = false
instead.tiny = true
local iface = std '@iface'
local type = std.type

local dict = {}

local function get_bool(o, nam)
	if type(o[nam]) == 'boolean' then
		return o[nam]
	end
	if type(o[nam]) == 'function' then
		return o:nam()
	end
	return nil
end

instead.inv_delim = '\n'
instead.hinv_delim = ' | '
instead.ways_delim = ' | '

instead.notitle = false
instead.noways = false
instead.noinv = false
instead.nopic = false

instead.fading_value = 0
function instead.mouse_filter()
	return 0
end

function instead.render_callback()
	return false
end

function instead.wait_use()
	return true
end

function instead.fading()
end

function instead.need_fading()
end

function instead.autosave(slot)
end

function instead.menu(n)
end

function instead.restart(v)
end

function instead.atleast(...)
	return true
end

function instead.version(...)
end

function instead.text_input()
	return false
end
function instead.screen_size() return 0,0 end

function iface:title(str) -- hide title
if type(str) ~= 'string' then return end
	return str

end

function iface:img() return '' end

function iface:imgl() return '' end

function iface:imgr() return '' end

function iface:anchor() return '' end

function iface:nb(t)
	return t == '' and ' ' or t
end

std.stat = std.class({
	__stat_type = true;
}, std.obj);

-- luacheck: globals menu
std.menu = std.class({
	__menu_type = true;
	new = function(_, v)
		if type(v) ~= 'table' then
			std.err ("Wrong argument to std.menu:"..std.tostr(v), 2)
		end
		v = std.obj(v)
--		std.setmt(v, self)
		return v
	end;
	inv = function(s, ...)
		local r, v
		if s.menu ~= nil then
			r, v = std.call(s, 'menu', ...) -- special method while in inv
		else
			r, v = std.call(s, 'act', ...) -- fallback to act
		end
		if not r and not v then
			return true, false -- menu mode
		end
		return r, v
	end;
}, std.obj);

std.setmt(std.phr, std.menu) -- make phrases menus
std.setmt(std.ref '@', std.menu) -- make xact menu

function iface:xref(str, o, ...)
	if type(str) ~= 'string' then
		std.err ("Wrong parameter to iface:xref: "..std.tostr(str), 2)
	end
	if not std.is_obj(o) or std.is_obj(o, 'stat') or o:disabled() then
		return str
	end
	local a = { ... }
	local args = ''
	for i = 1, #a do
		if type(a[i]) ~= 'string' and type(a[i]) ~= 'number' then
			std.err ("Wrong argument to iface:xref: "..std.tostr(a[i]), 2)
		end
		args = args .. ' '..std.dump(a[i])
	end
	local xref = std.string.format("%s%s", std.deref_str(o), args)
	-- std.string.format("%s%s", iface:esc(std.deref_str(o)), iface:esc(args))
	table.insert(dict, xref)
	xref = std.tostr(#dict)

	if std.cmd[1] == 'way' then
		return "[a]"..str..std.string.format("#%s", xref).."[/a]"
	elseif o:type 'menu' or std.is_system(o) then
		return "[a]"..str..std.string.format("#%s", xref).."[/a]"
	elseif std.cmd[1] == 'inv' then
		return "[a]"..str..std.string.format("#%s", xref).."[/a]"
	end
	return "[a]"..str..std.string.format("#%s", xref).."[/a]"
end

local iface_cmd = iface.cmd -- save old

function iface:cmd(inp)
	local a = std.split(inp)
	if std.tonum(a[1]) then
		std.table.insert(a, 1, 'act')
	end
	if a[1] == 'act' or a[1] == 'use' or a[1] == 'go' then
		if a[1] == 'use' then
			local use = std.split(a[2], ',')
			for i = 1, 2 do
				local u = std.tonum(use[i])
				if u then
					use[i] = dict[u]
				end
			end
			a[2] = std.join(use, ',')
		elseif std.tonum(a[2]) then
			a[2] = dict[std.tonum(a[2])]
		end
		inp = std.join(a)
	end
	return iface_cmd(self, inp)
end

std.obj { -- input object
	nam = '@input';
};

-- some aliases
menu = std.menu
stat = std.stat

-- fake sound
local function nop() end
instead.sound_load = nop -- instead_sound_load
instead.sound_free = nop -- instead_sound_free
instead.sounds_free = nop -- instead_sounds_free
instead.sound_channel = nop -- instead_sound_channel
instead.sound_volume = nop -- instead_sound_volume
instead.sound_panning = nop -- instead_sound_panning
instead.sound_load_mem = nop -- instead_sound_load_mem
instead.music_callback = nop -- instead_music_callback
instead.is_sound = function() return false end -- instead_sound


function instead.get_music()
	return instead.__music, instead.__music_loop
end

function instead.set_music(mus, loop)
	instead.__music = mus or nil
	instead.__music_loop = loop or 0
end

function instead.get_music_fading()
	return instead.__music_fadeout, instead.__music_fadein
end

function instead.set_music_fading(o, i)
	if not i then i = o end
	if o == 0 or not o then o = -1 end
	if i == 0 or not i then i = -1 end
	instead.__music_fadeout = o
	instead.__music_fadein = i
end

function instead.finish_music()
	if (instead.__music_loop or 0) == 0 then
		return false
	end
	instead.__music_loop = -1
	return true
end

function instead.get_sound()
	return instead.__sound, instead.__sound_channel, instead.__sound_loop
end

function instead.add_sound(s, chan, loop)
	if type(s) ~= 'string' then
		std.err("Wrong parameter to instead.add_sound()", 2)
	end
	if type(instead.__sound) ~= 'string' then
		return instead.set_sound(s, chan, loop)
	end
	if std.tonum(chan) then
		s = s..'@'..std.tostr(chan);
	end
	if std.tonum(loop) then
		s = s..','..std.tostr(loop)
	end
	instead.set_sound(instead.__sound..';'..s, instead.__sound_channel, instead.__sound);
end

function instead.set_sound(sound, chan, loop)
	instead.__sound = sound
	instead.__sound_loop = loop or 1
	instead.__sound_channel = chan or -1
end

function instead.stop_sound(chan, fo)
	local str = '@-1'

	if (chan and type(chan) ~= 'number') or (fo and type(fo) ~= 'number') then
		std.err("Wrong parameter to instead.stop_sound", 2)
	end

	if chan then
		str = '@'..std.tostr(chan)
	end

	if fo then
		str = str .. ',' .. std.tostr(fo)
	end
	return instead.add_sound(str);
end

function instead.stop_music()
	instead.set_music(nil, -1);
end

std.mod_done(function(s)
	instead.stop_music()
	instead.stop_sound() -- halt all
--	instead.sounds_free();
end)

local sounds = {}

std.mod_cmd(function(s)
	if std 'game':time() > 0 then
		sounds = {}
		instead.set_sound(); -- empty sound
	end
end)

-- aliases
local snd = {
	__gc = function(s)
		instead.sound_free(s.snd)
	end;
	__tostring = function(s)
		return s.snd
	end
}
snd.__index = snd;

function snd:play(...)
	instead.add_sound(self.snd, ...)
end

function snd:new(a, b, t)
	local o = {
		__save = function() end;
	}
	if type(a) == 'string' then
		o.snd = instead.sound_load(a);
	elseif type(t) == 'table' then
		o.snd = instead.sound_load_mem(a, b, t) -- hz, channel, t
	end
	if not o.snd then
		return
	end
	std.setmt(o, self)
	return std.proxy(o)
end

local sound = std.obj {
	nam = '@snd';
}

sound.set = instead.set_sound
sound.play = instead.add_sound
sound.stop = instead.stop_sound
sound.music = function(mus, loop)
	if mus == nil and loop == nil then
		return instead.get_music()
	end
	return instead.set_music(mus, loop)
end

sound.stop_music = instead.stop_music
sound.music_fading = function(o, i)
	if o == nil and i == nil then
		return instead.get_music_fading()
	end
	return instead.set_music_fading(o, i)
end

function sound.new(...)
	local s = snd:new(...)
	std.table.insert(sounds, s) -- avoid __gc in this step
	return s
end

function sound.music_callback(...)
	return instead.music_callback(...)
end

function sound.free(key)
	return instead.sound_free(key);
end

function sound.music_playing()
	return instead.__music ~= nil and instead.__music_loop ~= -1
end

function sound.playing(s,...)
	if type(s) ~= 'number' then
		return instead.is_sound()
	end
	return instead.sound_channel(s,...)
end

function sound.pan(c, l, r, ...)
	return instead.sound_panning(c, l, r, ...)
end

function sound.vol(v, ...)
	return instead.sound_volume(v, ...)
end

-- fake timer
std.obj {
	nam = '@timer';
	get = function(s)
		return s.__timer or 0;
	end;
	stop = function(s)
		return s:set(0)
	end;
	set = function(s, v)
		s.__timer = v
		return true
	end;
}

-- fake sprite
std.obj {
	nam = '@sprite';
	new = function() end;
	fnt = function() end;
	scr = function() end;
	direct = function() return false end;
}
-- fake pixels
std.obj {
	nam = '@pixels';
	fnt = function() end;
	new = function() end;
}
-- fake themes
local theme = std.obj {
	nam = '@theme';
	{
		win = { gfx = {}};
		inv = { gfx = {}};
		menu = { gfx = {}};
		gfx = {};
		snd = {};
	};
}

function theme.restore()
end

function theme.set()
end

function theme.reset()
end

function theme.name()
end

function theme.get()
end

function theme.win.reset()
end

function theme.win.geom()
end

function theme.win.color(f)
end

function theme.win.font()
end

function theme.win.gfx.reset()
end

function theme.win.gfx.up()
end

function theme.win.gfx.down()
end

function theme.inv.reset()
end

function theme.inv.geom()
end

function theme.inv.color(f)
end

function theme.inv.font()
end

function theme.inv.mode()
end

function theme.inv.gfx.reset()
end

function theme.inv.gfx.up()
end

function theme.inv.gfx.down()
end

function theme.menu.reset()
end

function theme.menu.bw(w)
end

function theme.menu.color()
end

function theme.menu.font()
end

function theme.menu.gfx.reset()
end

function theme.menu.gfx.button()
end;

function theme.gfx.reset()
end

function theme.gfx.cursor()
end

function theme.gfx.mode()
end

function theme.gfx.pad()
end

function theme.gfx.bg()
end

function theme.snd.reset()
end

function theme.snd.click()
end


std.mod_init(function()
	std.rawset(_G, 'instead', instead)
end)
local mp_hooked = false

std.mod_start(function()
	dict = {}
	local mp = std.ref '@metaparser'
	if mp then
		mp.winsize = 0
		mp.prompt = false
		if not mp_hooked then -- force truncate text for all commands
			std.mod_cmd(function(cmd) mp:trim() end)
			mp_hooked = true
		end
	end
end)

std.mod_step(function(state)
	if state then
		dict = {}
	end
end)