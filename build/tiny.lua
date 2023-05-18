--require "ext/gui"
-- some stubs for tiny-instead
-- stat, menu
-- fake audio
-- fake input
--if not prefs then prefs ={} end
if API == 'stead3' then
	require 'tiny3'
iface = std '@iface'
else
	require 'tiny2'
end
instead_busy =function() return 0 end
instead_direct=function() return 0 end
instead_autosave =function(n)
if not n then n="auto" end
iface:cmd("save "..instead_autosavepath().."/"..n..".sav")
end
instead_themespath =function() return "themes/" end
instead_cwdpath =function() return "../.." end
instead_theme_name = function() return '/default' end
instead_font_load =function() return "" end
stead.ticks =instead_ticks
get_ticks =instead_ticks
instead_mouse_pos =function() return 0,0 end
mouse_pos =instead_mouse_pos
instead_mouse_filter =function() return 0 end
instead_text_input =function() return 0 end
function instead_sprite_free() end
function instead_sprites_free() end
function instead_font_free() end
function instead_fonts_free() end
function instead_font_scaled_size()
return 1,1
end
function instead_sprite_alpha() end
function instead_sprite_colorkey() return "" end
function instead_sprite_copy(...) end
function instead_sprite_compose() end
function instead_sprite_draw() end
function instead_sprite_dup() end
function instead_sprite_load(name,...)
local arg ={...}
return type(name)=='string' and name or type(arg[1])=='string' and arg[1] or type(arg[2])=='string' and arg[2] or ""
end
function instead_sprite_pixel() return "" end
function instead_sprite_pixels()
return ""
end
function instead_sprite_scale() return 0
end
function instead_sprite_size() return 1,1
end
--function instead_sprite_text() return "" end
function instead_sprite_text_size() return 1,1 end
function instead_sprite_fill() end
--[[function instead_sound()
	return true
end
is_sound = function()
  return false
end
instead_sound_channel =function() end]]
function instead_theme_var(a,b) return "1" end
function instead_screen_size() return 0,0 end
function instead_screen_dpi() return 0 end
function hideinv() end
-- fake themes
local function getstr(str)
if stead.type(str) ~= 'string' then return '' end
return str
end
function iface:esc(str)
	--str = str:gsub("\\?[\\<>]", { ['\\\\'] = '\\\\\\\\\\', ['>'] = iface:nb('>'), ['<'] = iface:nb('<') })
	return getstr(str)
end
function iface:title(str)
if type(str) ~= 'string' then return end
	return getstr(str)
end
function iface:img(str)
return "<img></img>"
end
function iface:imgl(str) return iface:img(str) end
function iface:imgr(str) return iface:img(str) end
function iface:anchor() return '' end
function iface:enum(n,str)
	if str == nil or n == nil then return nil; end;
	return n..self:nb(' - ')..str;
end
function iface:nb(t)
return getstr(t)
end
function iface:under(str)
	return getstr(str)
end
function iface:em(str)
	return getstr(str)
end
function iface:st(str)
return getstr(str)
end
function iface:right(str)
return getstr(str)
end
function iface:left(str)
return getstr(str)
end
function iface:center(str)
return getstr(str)
end
function iface:just(str)
return getstr(str)
end
function iface:tab(str,al)
if stead.tonum(str) then
		str = stead.tostr(str)
	end
	if stead.type(str) ~= 'string' then
		return nil;
	end
	--[[if al == 'right' then
		str = str .. ",right"
	elseif al == 'center' then
		str = str .. ",center"
	end]]
	return str
end;
function iface:y(str,al)
	if stead.tonum(str) then
		str = stead.tostr(str)
	end
	if stead.type(str) ~= 'string' then
		return nil;
	end
	--[[if al == 'middle' then
		str = str .. ",middle"
	elseif al == 'top' then
		str = str .. ",top"
	end]]
	return str
end
function iface:bold(str)
return getstr(str)
end
function iface:top(str)
return getstr(str)
end
function iface:bottom(str)
return getstr(str)
end
function iface:middle(str)
return getstr(str)
end
--Форматирование текста
steadfmt =stead.fmt
function stead.fmt(...)
local str,rep =steadfmt(...),0
if (not str) then return "" end
str =string.gsub(str,"<w:(.-)>","%1")
local regexp ="<(.-).->(.-)</%1>"
str,rep =string.gsub(str,regexp,"%2")
--Заменяем вложенные теги тоже.
while rep >0 do
str,rep =string.gsub(str,regexp,"%2")
end
return str
end
instead_render_callback =function() end
tiny =true
require "ext/paths"
require "ext/sound"
--require "ext/sprites"
require "ext/timer"
