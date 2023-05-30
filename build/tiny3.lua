local std = stead

local instead = std.obj { nam = '@instead'; version_table = {3, 5, 1};}
	std.savepath = instead.savepath
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
instead.clipboard=instead_clipboard
instead.theme_name=instead_theme_name
instead.ticks = instead_ticks
instead.direct=function() return instead_direct() end
instead.font_load = instead_font_load
instead.font_free = instead_font_free
instead.font_scaled_size = instead_font_scaled_size
instead.sprite_alpha = instead_sprite_alpha
instead.sprite_dup = instead_sprite_dup
instead.sprite_scale = instead_sprite_scale
instead.sprite_rotate = instead_sprite_rotate
instead.sprite_text = instead_sprite_text
instead.sprite_text_size = instead_sprite_text_size
instead.sprite_draw = instead_sprite_draw
instead.sprite_copy = instead_sprite_copy
instead.sprite_compose = instead_sprite_compose
instead.sprite_fill = instead_sprite_fill
instead.sprite_pixel = instead_sprite_pixel
instead.sprite_load = instead_sprite_load
instead.sprite_free = instead_sprite_free
instead.sprite_size = instead_sprite_size
instead.sprites_free = instead_sprites_free
instead.sprite_colorkey = instead_sprite_colorkey
instead.sprite_pixels = instead_sprite_pixels
instead.mouse_pos = function() return instead_mouse_pos() end
instead.mouse_show = function() instead_mouse_show() end
instead.mouse_filter = instead_mouse_filter
instead.text_input =instead_text_input
instead.finger_pos = instead_finger_pos

instead.noise1 = instead_noise1
instead.noise2 = instead_noise2
instead.noise3 = instead_noise3
instead.noise4 = instead_noise4

instead.render_callback = instead_render_callback

instead.direct = instead_direct
--std.busy = instead_busy
function instead.mouse_filter()
	return 0
end
function instead.grab_events() return true end
function instead.wait_use()
	return true
end

function instead.fading()
end

function instead.need_fading()
end

--[[function instead.autosave(slot)
end

function instead.menu(n)
end

function instead.restart(v)
end]]
instead.restart =instead_restart
instead.menu =instead_menu
instead.autosave =instead_autosave
function instead.atleast(...)
	return true
end

function instead.version(...)
	if #{...} == 0 then
		return instead.version_table
	end
	if not instead.atleast(...) then
		local v = false
		for _, n in std.ipairs({...}) do
			if std.type(n) ~= 'number' then
				std.err([[Wrong instead.version argument: ]]..std.tostr(n), 2)
			end
			v = (v and (v .. '.') or '').. std.tostr(n)
		end
		std.err ([[The game requires instead engine of version ]] ..(v or '???').. [[ or higher.
		https://instead-hub.github.io]], 2)
	end
end
instead.screen_size = function() return instead_screen_size() end
instead.screen_dpi = instead_screen_dpi

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
std.obj { -- input object
	nam = '@input';
};
function iface:input(event, ...)
	local input = std.ref '@input'
	if type(input) ~= 'table' then
		return
	end
	if event == 'kbd' then
		if type(input.key) == 'function' then
			return input:key(...); -- pressed, event
		end
	elseif event == 'mouse' then
		if type(input.click) == 'function' then
			return input:click(...); -- pressed, x, y, mb
		end
	elseif event == 'wheel' then
		if type(input.wheel) == 'function' then
			return input:wheel(...);
		end
	elseif event == 'finger' then
		if type(input.finger) == 'function' then
			return input:finger(...); -- pressed, x, y, finger
		end
	elseif event == 'event' then
		if type(input.event) == 'function' then
			return input:event(...);
		end
	elseif event == 'text' then
		if type(input.text) == 'function' then
			return input:text(...);
		end
	end
	return
end

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
	if not dict[xref] then
table.insert(dict, xref)
		dict[xref] = #dict
end
	xref = std.tostr(dict[xref])
if o:type 'menu' or std.is_system(o) then
		return "[a]"..str..std.string.format("#%s", -xref).."[/a]"
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

	instead.get_picture = function()
		local s = stead.call(std.here(), 'pic')
		if not s then
			s = stead.call(std.ref 'game', 'pic')
		end
		return s and std.tostr(s)
	end

-- fake sound
local function nop() end
--[[instead.sound_load = nop -- instead_sound_load
instead.sound_free = nop -- instead_sound_free
instead.sounds_free = nop -- instead_sounds_free
instead.sound_channel = nop -- instead_sound_channel
instead.sound_volume = nop -- instead_sound_volume
instead.sound_panning = nop -- instead_sound_panning
instead.sound_load_mem = nop -- instead_sound_load_mem
instead.music_callback = nop -- instead_music_callback
instead.is_sound = function() return false end -- instead_sound]]
instead.sound_load = instead_sound_load
instead.sound_free = instead_sound_free
instead.sounds_free = instead_sounds_free
instead.sound_channel = instead_sound_channel
instead.sound_volume = instead_sound_volume
instead.sound_panning = instead_sound_panning
instead.sound_load_mem = instead_sound_load_mem
instead.music_callback = instead_music_callback
instead.is_sound = instead_sound

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
local spr_get = function(s)
	if type(s) == 'string' then
		return s
	end
	return std.tostr(s)
end
local spr ={
	__gc = function(s)
		instead_sprite_free(s.spr)
	end;
	__tostring = function(s)
		return s.spr
	end;
}
spr.__index = spr
 function spr:new(nam)
 	if type(nam) ~= 'string' then
		std.err("Wrong argument to spr:new(): "..std.tostr(nam), 2)
	end
 	local o = {
		spr = nam;
		__save = function() end;
	}
	std.setmt(o, self)
	return std.proxy(o)
end
function spr:alpha(alpha, ...)
	return spr:new(instead.sprite_alpha(self.spr, alpha, ...));
end;
function spr:colorkey(color, ...)
	instead_sprite_colorkey(self.spr, color, ...);
	return self
end;
function spr:dup(...)
	return spr:new(instead_sprite_dup(self.spr, ...));
end;
function spr:scale(xs, ys, smooth, ...)
	if smooth == nil then
		smooth = true -- default is on
	end;
	return spr:new(instead_sprite_scale(self.spr, xs, ys, smooth,...));
end;
function spr:rotate(angle, smooth, ...)
	if smooth == nil then
		smooth = true -- default is on
	end;
	return spr:new(instead_sprite_rotate(self.spr, angle, smooth, ...));
end;
function spr:size()
	return instead_sprite_size(self.spr);
end;
function spr:draw(fx, fy, fw, fh, d, x, y, alpha)
	if d == nil and x == nil and y == nil then
		d, x, y, alpha = fx, fy, fw, fh
		fx, fy, fw, fh = 0, 0, -1, -1
	end;
	instead_sprite_draw(self.spr, fx, fy, fw, fh, spr_get(d), x, y, alpha);
	return d
end;
function spr:copy(fx, fy, fw, fh, d, x, y)
	if d == nil and x == nil and y == nil then
		d, x, y = fx, fy, fw
		fx, fy, fw, fh = 0, 0, -1, -1
	end;
	instead_sprite_copy(self.spr, fx, fy, fw, fh, spr_get(d), x, y);
	return d
end;
function spr:compose(fx, fy, fw, fh, d, x, y)
	if d == nil and x == nil and y == nil then
		d, x, y = fx, fy, fw
		fx, fy, fw, fh = 0, 0, -1, -1
	end;
	instead_sprite_compose(self.spr, fx, fy, fw, fh, spr_get(d), x, y);
	return d
end;
function spr:fill(x, y, w, h, col)
	if h == nil and col == nil then
		instead_sprite_fill(self.spr, 0, 0, -1, -1, x);
		return self
	end;
end;
	function spr:pixel(x, y, col, alpha)
	if not col then
		return instead_sprite_pixel(self.spr, x, y, col, alpha)
	end;
	instead_sprite_pixel(self.spr, x, y, col, alpha)
	return self
end;
local fnt ={
	__gc = function(s)
		instead_font_free(s.fnt)
	end;
height =function()
return 1
end;
	__tostring = function(s)
		return s.fnt
	end;
}
fnt.__index = fnt
function fnt:new(nam)
	if type(nam) ~= 'string' then
		std.err("Wrong argument to fnt:new(): "..std.tostr(nam), 2)
	end
	local o = {
		fnt = nam;
		__save = function() end;
	}
	std.setmt(o, self)
	return std.proxy(o)
end
function fnt:size(...) return instead_sprite_text_size(self.fnt, ...) end
function fnt:text(...) return spr:new(instead_sprite_text(self,...))
end
 local sprite =std.obj {
	nam = '@sprite';
	new = function(w,h,...) 
	if std.tonum(w) and std.tonum(h) then
		local t = 'blank:'..std.tostr(std.math.floor(w))..'x'..std.tostr(std.math.floor(h))
		return spr:new(instead_sprite_load(t))
	end
	local sp = instead_sprite_load(w, h, ...)
	if not sp then
		std.err("Can not load sprite: "..std.tostr(w), 2);
	end
	return spr:new(sp)
	end;
	fnt = function(name, sz, ...) 
	if not std.tonum(sz) then
		std.err("No font size specified in sprite:fnt().", 2)
	end;
	local fn = instead_font_load(name, sz, ...)
	if not fn then
		std.err("Can not load font: "..std.tostr(name), 2);
	end;
	return fnt:new(fn)
end;
font_scaled_size =function() return instead_font_scale_size() end;
render_callback =function() instead_render_callback() end;
	direct = function(v) return instead_direct(v) end;
scr =function() return spr:new('screen') end;
}

-- fake pixels
local pfnt = {
}

pfnt.__index = pfnt
std.setmt(pfnt, fnt)

local pxl = {
	__gc = function(s)
	end;
	__tostring = function(s)
		return s.p
	end;
}

pxl.__index = pxl

function pxl:dup()
	local w, h, s = self:size()
	local p = instead.sprite_pixels(w, h, s)
	if p then
		self:copy(p)
	end
	return self:new(p)
end

function pxl:sprite(...)
	return sprite.new(self, false, ...)
end

function pxl:tosprite(...)
	return sprite.new(self, true, ...)
end

function pxl:draw_spr(fx, fy, fw, fh, d, x, y, alpha)
	if d == nil and x == nil and y == nil then
		instead.sprite_draw(self, 0, 0, -1, -1, spr_get(fx), fy, fw, fh);
		return fx
	end
	instead.sprite_draw(self, fx, fy, fw, fh, spr_get(d), x, y, alpha);
	return d
end

function pxl:copy_spr(fx, fy, fw, fh, d, x, y, alpha)
	if d == nil and x == nil and y == nil then
		instead.sprite_copy(self, 0, 0, -1, -1, spr_get(fx), fy, fw, fh);
		return fx
	end
	instead.sprite_copy(self, fx, fy, fw, fh, spr_get(d), x, y, alpha);
	return d
end

function pxl:compose_spr(fx, fy, fw, fh, d, x, y, alpha)
	if d == nil and x == nil and y == nil then
		instead.sprite_compose(self, 0, 0, -1, -1, spr_get(fx), fy, fw, fh);
		return fx
	end
	instead.sprite_compose(self, fx, fy, fw, fh, spr_get(d), x, y, alpha);
	return d
end

function pxl:scale(...)
	return pxl:new(self:new_scaled(...))
end

function pxl:rotate(...)
	return pxl:new(self:new_rotated(...))
end

local function poly(self, fn, t, ...)
	if type(t) ~= 'table' then
		std.err("Wrong argument to :poly()", 3)
	end
	if #t < 4 then
		return
	end
	local n = #t
	for i = 1, n, 2 do
		if i == n - 1 then
			fn(self, t[i], t[i+1], t[1], t[2], ...);
		else
			fn(self, t[i], t[i+1], t[i+2], t[i+3], ...);
		end
	end
end

function pxl:poly(t, ...)
	--poly(self, self.line, t, ...)
end

function pxl:polyAA(t, ...)
--poly(self, self.lineAA, t, ...)
end
function pxl:fill_poly() end
function pxl:val(...) return 0,0,1,1 end
function pxl:size() return 1,1 end
function pxl:fill() end
function pxl:new(p)
	if type(p) ~= 'string' then
		std.err("Wrong argument to pxl:new(): "..std.tostr(nam), 2)
	end
 	local o = {
		pxl = p;
		__save = function() end;
	}
	std.setmt(o, self)
	return std.proxy(o)
end

function pfnt:new(nam)
	return fnt.new(self, nam)
end

function pfnt:text(text, col, style, ...)
	local s = self
	return pxl:new(instead.sprite_pixels(instead.sprite_text(s.fnt, text, col, style, ...)))
end
local pixels =std.obj {
	nam = '@pixels';
}
function pixels.fnt(name, sz, ...)
	if not std.tonum(sz) then
		std.err("No font size specified.", 2)
	end
	return pfnt:new(instead.font_load(name, -sz, ...))
end

function pixels.new(...)
	return pxl:new(instead_sprite_pixels(...))
end
local theme = std	.obj {
	nam = '@theme';
	vars = {};
	reset_vars = {};
			{
win = { gfx = {}};
		inv = { gfx = {}};
		menu = { gfx = {}};
		gfx = {};
		snd = {};
				scr = {};
	};
}

function theme.restore(name)
end

function theme.set(name,val)
end

function theme.reset(name)
end

function theme.name()
return instead_theme_name()
end

function theme.get(...)
return instead_theme_var(...)
end
function theme.scr.w()
return 0
end
function theme.scr.h()
return 0
end
function theme.win.reset()
end

function theme.win.geom(x,y,w,h)
end

function theme.win.color(fg, link, alink)
end

function theme.win.font(name, size, height)
end

function theme.win.gfx.reset()
end

function theme.win.gfx.up(pic, x, y)
end

function theme.win.gfx.down(pic, x, y)
end

function theme.inv.reset()
end

function theme.inv.geom(x, y, w, h)
end

function theme.inv.color(fg, link, alink)
end

function theme.inv.font(name, size, height)
end

function theme.inv.mode()
end

function theme.inv.gfx.reset()
end

function theme.inv.gfx.up(pic, x, y)
end

function theme.inv.gfx.down(pic, x, y)
end

function theme.menu.reset()
end

function theme.menu.bw(w)
end

function theme.menu.color(fg, bg, alpha, link, alink)
end

function theme.menu.font(name, size, height)
end

function theme.menu.gfx.reset()
end

function theme.menu.gfx.button(b, x, y)
end;

function theme.gfx.reset()
end

function theme.gfx.cursor(norm, use, x, y)
end

function theme.gfx.mode(mode)
end

function theme.gfx.pad()
end

function theme.gfx.bg(bg)
end

function theme.snd.reset()
end

function theme.snd.click(w)
end

local mp_hooked = false

std.mod_start(function()
	dict = {}
		local mp = std.ref '@metaparser'
		if mp then
			mp.msg.CUTSCENE_MORE = '^'..mp.msg.CUTSCENE_HELP
		mp.winsize = 0
		mp.prompt = false
		if not mp_hooked then -- force truncate text for all commands
			std.mod_cmd(function(cmd) mp:trim() end)
			mp_hooked = true
		end
		end
	end)
		std.mod_init(function()
			std.rawset(_G, 'instead', instead)
--require("ext/sandbox")
end)
std.mod_step(function(state)
	if state then
		dict = {}
	end
end)
--require("ext/paths")