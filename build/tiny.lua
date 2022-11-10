-- some stubs for tiny-instead
-- fake game.gui
-- stat, menu
-- fake audio
-- fake input

if API == 'stead3' then
	require 'tiny3'
	require "ext/sound"
	local instead = std '@instead'
	local iface = std '@iface'
	instead.music_callback = function() end
	instead.restart = instead_restart
	instead.menu = instead_menu
	instead.savepath = function() return "../.." end
	std.savepath = instead.savepath
	function iface:em(str)
		if type(str) == 'string' then
			return str
		end
	end
	function iface:bold(str)
		if type(str) == 'string' then
			return str
		end
	end
	instead.get_picture = function()
		local s = stead.call(std.here(), 'pic')
		if not s then
			s = stead.call(std.ref 'game', 'pic')
		end
		return s and std.tostr(s)
	end
	std.mod_start(function()
		std.mod_init(function()
			std.rawset(_G, 'instead', instead)
			--require "ext/sandbox"
		end)
	end)
else
	require 'tiny2'
end
