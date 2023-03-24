 -- some stubs for tiny-instead
-- stat, menu
-- fake audio
-- fake input
--if not prefs then prefs ={} end
if API == 'stead3' then
	require 'tiny3'
else
	require 'tiny2'
end
instead_theme_name = function() return '/default' end
instead_font_load =function() return {} end
stead.ticks =instead_ticks
get_ticks =instead_ticks
mouse_pos =function() return 0,0 end
function instead_sprite_free() end
function instead_sprites_free() end
function instead_font_free() end
function instead_fonts_free() end
function instead_font_scaled_size()
return 0
end
function instead_sprite_alpha() end
function instead_sprite_colorkey() end
function instead_sprite_copy(...) end
function instead_sprite_compose() end
function instead_sprite_draw() end
function instead_sprite_dup() end
function instead_sprite_load() return "" end
function instead_sprite_pixel() end
function instead_sprite_scale() return 0
end
function instead_sprite_size() return 0,0
end
--function instead_sprite_text() return "" end
function instead_sprite_text_size() return 0,0 end
function instead_sprite_fill() end
function instead_sound()
	return true
end
is_sound = function()
  return false
end
instead_sound_channel =function() end
function instead_theme_var(a,b) return 1,"" end
function hideinv() end
-- fake themes
--Форматирование текста
steadfmt=stead.fmt
stead.fmt=function(...)
local str =steadfmt(...)
if(not str) then return end
str =stead.string.gsub(str,"<([%w%d].-)>(.-)</%1>","%2")
return str
end
tiny =true
require "ext/paths"
require "ext/sound"
require "ext/timer"
