-- $Name: Модуль URQ$
-- $Version: 2.0$
set_music('mus/vibe-ew.it')
main = room {
	nam = 'Модуль URQ v2.0',
	dsc = function(s)
		pn (txtc(txtu(txtnb[[Этот компонент не является игрой!]])))
		pn ()
		pn [[ Модуль URQ позволяет играть в квесты, написанные на URQL.]]
		pn ()
		pn [[ http://instead.syscall.ru ]]
		pn [[ http://instead.googlecode.com ]]
		pn ()
		pn (txtc(txtb[[ Информация для разработчиков и опытных пользователей: ]]))
		pn ()
		pn [[ Для того, чтобы самостоятельно добавить URQ игру в INSTEAD Вам необходимо: ]]
		pn ()
		pn [[1) Распаковать игру в отдельный каталог в каталоге с играми для INSTEAD.]]
		pn ()
		pn [[2) Добавить в каталог с игрой файл 'main.lua' с следующим содержимым:]]
		pn ()
		pn (txtnb(" ")..[[-- $Name: Название игры в кодировке UTF-8$]])
		pn (txtnb(" ")..[[dofile "../urq/urq.lua"]])
		pn (txtnb(" ")..[[lurq "имя_файла.qst"]])
		pn ()
		pn [[3) При необходимости, провести дополнительную настройку модуля. Для этого
		перед командой lurq (загрузка URQ квеста) можно задать следующие параметры:]]
		pn ()
		pn (txtl[[ urq.extension_strings = false -- не поддерживать строковое расширение;]])
		pn ()
		pn (txtl[[ urq.extension_furq = false -- не поддерживать расширения FireURQ;]])
		pn ()
		pn (txtl[[ urq.extension_global_else = true -- перед оператором else не нужен разделитель;]])
		pn ()
		pn (txtl[[ urq.extension_print_spaces = true -- не убирать пробелы перед строкой в операторах p/pln;]])
		pn ()
		pn (txtl[[ urq.extension_input = false -- не поддерживать ввод и pause;]])
		pn ()
		pn (txtl[[ urq.extension_useinv = true -- поддерживать операцию "осмотреть инвентарь";]])
		pn ()
		pn (txtl[[ urq.extension_invproc = true -- вызывать код локации еще раз при использовании инвентаря;]])
		pn ()
		pn (txtl[[ urq.extension_varimage = false -- не поддерживать переменную image;]])
		pn ()
		pn (txtl[[ urq.extension_loccnt = false -- не поддерживать count_ (вместо этого, работать в стиле ripos urq);]])
		pn ()
		pn (txtl[[ urq.extension_loccnt_btn = false -- изменять счетчики посещений локаций не только в btn но и 
			в любых переходах;]])
		pn (txtl[[ urq.extension_pause_btn = false -- не показывать кнопки во время пауз;]])
		pn ()
		pn (txtl[[ urq.extension_input_btn = true -- показывать кнопки во время ввода;]])
		pn ()
		pn (txtl([[ game.codepage="CP866" -- для игр в DOS кодировке.]]))
		pn ()
		pn (txtem[[При возникновении проблем с запуском URQ игр, обращайтесь на наш форум: http://instead.syscall.ru/forum]])
		pn ()
		pn (txtr("INSTEAD 2009-2011"))
	end
};
