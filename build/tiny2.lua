-- some stubs for tiny-instead
-- fake game.gui
-- stat, menu
-- fake audio
-- fake input
-- show hints (numbers)
game.hinting = true

-- fake gui
game.gui = {
	fading = 4;
	ways_delim = ' | ';
	inv_delim = ' | ';
	hinv_delim = ' | ';
	hideways = false;
	hideinv = false;
	hidetitle = false;
}

-- menu and stat
stat = function(v)
	v.status_type = true
	v.id = false
	return obj(v)
end

function isStatus(v)
	if stead.type(v) ~= 'table' then
		return false
	end
	if v.status_type then
		return true
	end
	return false
end

function isMenu(v)
	if stead.type(v) ~= 'table' then
		return false
	end
	if v.menu_type then
		return true
	end
	return false
end

stead.menu_save = function(self, name, h, need)
	local dsc;
	if need then
		print ("Warning: menu "..name.." can not be saved!");
		return
	end
	stead.savemembers(h, self, name, need);
end

function menu(v)
	v.menu_type = true
	if v.inv == nil then
		v.inv = function(s)
			local r,v
			r, v = stead.call(s, 'menu');
			if v == nil then v = true end
			if r == nil then
				stead.obj_tag(stead.me(), MENU_TAG_ID); -- retag menu field
			end
			return r, v
		end
	end
	if v.act == nil then
		v.act = function(s)
			local r,v
			r,v = stead.call(s, 'menu');
			if v == nil then v = true end
			if r == nil then
				stead.obj_tag(stead.me(), MENU_TAG_ID); -- retag menu field
			end
			return r, v
		end
	end
	if v.save == nil then
		v.save = stead.menu_save;
	end
	return obj(v);
end

-- Audio
stead.get_music = function()
	return game._music, game._music_loop;
end

stead.get_music_loop = function()
	return game._music_loop;
end

stead.save_music = function(s)
	if s == nil then
		s = self
	end
	s.__old_music__ = stead.get_music();
	s.__old_loop__ = stead.get_music_loop();
end

stead.restore_music = function(s)
	if s == nil then
		s = self
	end
	stead.set_music(s.__old_music__, s.__old_loop__);
end

stead.set_music = function(s, count)
	game._music = s;
	if not stead.tonum(count) then
		game._music_loop = 0;
	else
		game._music_loop = stead.tonum(count);
	end
end

stead.set_music_fading = function(o, i)
end

stead.get_music_fading = function()
end

stead.stop_music = function()
	stead.set_music(nil, -1);
end

stead.is_music = function()
	return game._music ~= nil and game._music_loop ~= -1
end

function instead_sound()
	return true
end

--пока нет звуков
stead.is_sound = function()
  return false
end

is_sound = function()
  return false
end

stead.get_sound = function()
	return game._sound, game._sound_channel, game._sound_loop; 
end

stead.get_sound_chan = function()
	return game._sound_channel
end

stead.get_sound_loop = function()
	return game._sound_loop
end

stead.stop_sound = function(chan, fo)
	if not stead.tonum(chan) then
		if stead.tonum(fo) then
			stead.set_sound('@-1,'..stead.tostr(fo));
		else
			stead.set_sound('@-1');
		end
		return
	end
	if stead.tonum(fo) then
		stead.add_sound('@'..stead.tostr(chan)..','..stead.tostr(fo));
	else
		stead.add_sound('@'..stead.tostr(chan));
	end
end

stead.add_sound = function(s, chan, loop)
   if stead.type(s) ~= 'string' then
		return
	end
	if stead.type(game._sound) == 'string' then
		if stead.tonum(chan) then
			s = s..'@'..stead.tostr(chan);
		end
		if stead.tonum(loop) then
			s = s..','..stead.tostr(loop)
		end
		stead.set_sound(game._sound..';'..s, stead.get_sound_chan(), stead.get_sound_loop());
	else
		stead.set_sound(s, chan, loop);
	end
end

stead.set_sound = function(s, chan, loop)
  game._sound = s;
	if not stead.tonum(loop) then
		game._sound_loop = 1;
	else
		game._sound_loop = stead.tonum(loop);
	end

	if not stead.tonum(chan) then
		game._sound_channel = -1;
	else
		game._sound_channel = stead.tonum(chan);
	end
end

-- those are sill in global space
add_sound = stead.add_sound
set_sound = stead.set_sound
stop_sound = stead.stop_sound

get_sound = stead.get_sound
get_sound_loop = stead.get_sound_loop
get_sound_chan = stead.get_sound_chan

get_music = stead.get_music
get_music_fading = stead.get_music_fading
get_music_loop = stead.get_music_loop

set_music = stead.set_music
set_music_fading = stead.set_music_fading
stop_music = stead.stop_music

save_music = stead.save_music
restore_music = stead.restore_music

is_music = stead.is_music

--Для космический рейнджеров
unpack = stead.unpack;

stead.set_timer = function() end

stead.timer = function()
	return
end
instead_theme_name = function() return 'default' end

stead.module_init(function(s) 
	timer = obj {
		nam = 'timer',
		get = function(s)
			return 0
		end,
		stop = function(s)
		end,
		del = function(s)
		end,
		set = function(s, v)
			return true
		end,
	};
end)

stead.objects.input =  function()
	return obj { -- input object
		system_type = true,
		nam = 'input',
	};
end;

--Место сохранений
function instead_savepath()
    return "../../" --Разрешаем сохранять на 2 уровня выше
end

stead.list_search = function(self, n, dis)
	local i
	if stead.tonum(n) then
		i = self:byid(stead.tonum(n), dis);
	end
	if not i then
		i = self:look(n)
		if not i then
			i = self:name(n, dis);
		end
		if not i then
			return nil
		end
	end
	if not dis and isDisabled(stead.ref(self[i])) then
		return nil
	end
	return self[i], i
end

local dict = {}

iface.xref = function(self, str, obj, ...)
	local cmd = ''
	local o = stead.ref(obj)

	if not isObject(o) or isStatus(o) or (not o.id and not isXaction(o)) then
		if isStatus(o) then
			str = string.gsub(str, "%^", " ")
			return ("[a]"..(str or '').."#0[/a]")
		end
		return str
	end

	if stead.cmd == 'way' then
		cmd = 'go'
	elseif stead.cmd == 'inv' then
		cmd = 'use'
	elseif isSceneUse(o) then
		cmd = 'use'
	elseif isXaction(o) and not o.id then
		cmd = 'act'
	elseif stead.ref(stead.here():srch(obj)) then
		cmd = 'act'
	end

	local a = ''
	local varg = {...}
	for i = 1, stead.table.maxn(varg) do
		a = a..','..varg[i]
	end

	local n = stead.deref(obj)
	local xref = string.format("%s%s", stead.tostr(o.id or n), a)
	if not dict[xref] then
		stead.table.insert(dict, { xref, cmd } )
		dict[xref] = #dict
	end
	xref = dict[xref]

	local n = xref
	if isMenu(o) then
		n = n + 1000 -- ???
	end
--	if isXaction(o) and not o.id then
--		return stead.cat('[a:'..stead.tostr(n)..']',str,'[/a]')
--	end
	return stead.cat('[a]', (str or ''), '#', stead.tostr(n), '[/a]');
end

local tag_all = stead.player_tagall -- save old

stead.player_tagall = function(self)
	dict = {}
	return tag_all(self)
end

local iface_cmd = iface.cmd -- save old

function iface:cmd(inp)
	local cmd, a = stead.getcmd(inp)
	if stead.tonum(cmd) then
		stead.table.insert(a, 1, cmd)
		cmd = 'act'
	end
	if cmd == 'act' or cmd == 'use' or cmd == 'go' then
		if a[1] == '' then -- fix use vs look
			return iface_cmd(self, "look")
		end
		if cmd == 'use' then
			local use = { }
			local c = false
			for i = 1, #a do
				local u = stead.tonum(a[i])
				u = u and dict[u]
				c = c or (u and u[2])
				stead.table.insert(use, u and u[1] or a[i])
			end
			if #a == 1 and c == 'act' then -- fix use 1,2 where 1,2 is object and look
				cmd = 'act'
			end
			inp = cmd .. ','..stead.table.concat(use, ',')
		elseif stead.tonum(a[1]) then
			local d = dict[stead.tonum(a[1])]
			a[1] = d and d[1] or a[1]
			inp = cmd .. ','..stead.table.concat(a, ',')
		end
	end
	return iface_cmd(self, inp)
end

--Форматирование текста
stead.fmt = function(...)
	local res
	local a = {...}

	for i = 1, stead.table.maxn(a) do
		if stead.type(a[i]) == 'string' then
			local s = stead.string.gsub(a[i],'\t', stead.space_delim):gsub('[\n]+', stead.space_delim);
			s = stead.string.gsub(s, '\\?[\\^]', { ['^'] = '\n', ['\\^'] = '^',
				['\\\\'] = '\\' });
			res = stead.par('', res, s);
			--Убирание информации из тегов
			res = stead.string.gsub(res,"<u.->(.-)</u>","%1");
			res = stead.string.gsub(res,"<b.->(.-)</b>","%1");
			res = stead.string.gsub(res,"<l.->(.-)</l>","%1");
			res = stead.string.gsub(res,"<i.->(.-)</i>","%1");
			res = stead.string.gsub(res,"<w:(.-)>","%1");
			res = stead.string.gsub(res,"<c.->(.-)</c>","%1");
		end
	end
	return res
end

function iface.title(t)
	if type(t) ~= 'string' then return end
	return '['..t..']'
end
