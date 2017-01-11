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


--Fake timer
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

--Мой iface
iface.xref = function(self, str, obj, ...)
		local o = stead.ref(stead.here():srch(obj));
		if not o then 
			o = stead.ref(ways():srch(obj));
		end
		if not o then
			o = stead.ref(stead.me():srch(obj));
		end
		o = stead.ref(obj);
		local a = ''
	    local varg = {...}
	    for i = 1, stead.table.maxn(varg) do
		   a = a..','..varg[i]
	    end
	    if isXaction(o) and not o.id then
		   return stead.cat('[a:'..stead.deref(obj)..a..']',str,'[/a]');
	    end
		
		if not o or not o.id then
		    if isStatus(o) then
			    str = string.gsub(str, "%^", " ");
                return ("[a]"..(str or '').."#0[/a]");
            end
			return str;
		end	
		local n = stead.tonum(stead.nameof(o))
--      Новое отображение ссылок, для меню сдвиг на 1000
		if (isMenu(o)) then 
			return ("[a]"..(str or '').."#"..stead.tostr((n or o.id)+1000).."[/a]"); 
		end
		return ("[a]"..(str or '').."#"..stead.tostr(n or o.id).."[/a]");
end;

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