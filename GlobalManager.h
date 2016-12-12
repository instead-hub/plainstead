#pragma once

#include <vector>

class GlobalManager
{
public:
	static GlobalManager& getInstance() {
        static GlobalManager instance;
        return instance;
    }
	void userStartGame(); //ѕользователь начал игру
	void userSavedFile();  //ѕользователь сохранил файл
	void userNewCommand(); //выполнение новой команды пользователем
	bool isUserSaveLastFile(); //ѕризнак того, что пользователь сохранил игру
	bool isUserStartGame();
	void appendCommandAndRespond(CString command, CString respond); //добавление команды с откликом
	void appendCommand(CString command); //добавить новую команду
	void appendLastRespond(CString command); //добавить последний ответ на команду
	CString fullHistoryData(); //вс€ истори€ целиком (команда+ответ)
	CString previosHistoryData(); //предыдущий элемент в истории (команда+ответ)
	bool previosHistoryHave(); //наличие предыдущего элемента
	CString nextHistoryData(); //следующий элемент в истории (команда+ответ)
	bool nextHistoryHave(); //наличие следующего элемента
	void enableHistory(bool isEn); //включение записи истории

	CString commandData(); //команда в истории
	bool previosCommandMove(); //наличие предыдущей команда
	bool nextCommandMove(); //наличие следующей команды

	bool isIgnoreExitDialog; //игнорировать диалог на выходе
	bool isUseMenu(); //»спользуетс€ ли меню
	bool isAutoMenuDetect(); //јвтоматическое определение меню
	CString keyMenuString(); // лючева€ строка дл€ определени€ меню
	void setUseMenu();
	static int lastString;
private:
	GlobalManager();
	~GlobalManager();
	GlobalManager( const GlobalManager&);  
    GlobalManager& operator=( GlobalManager& );

	bool m_haveFileSaved;
	bool m_userStartGame;
	std::vector< std::pair<CString/*command*/,CString/*respond*/> > commandHistory; //истори€ команд
	int m_currHistPos; //текущий указатель в истории
	int m_currCmdPos; //текущий указатель в командах
	bool m_useMenu;
	bool m_en_history;
};
