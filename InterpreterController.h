#pragma once

class InterpreterController
{
public:
    InterpreterController(void);
    ~InterpreterController(void);
    static void startGameFile(CString gameFile, CString gameName, int autolog);//установить игровой файл
    static CString RunInterpreter(CString command); //Запуск интерпретатора, с определенной игрой и командой на обработку
	static void endInterpreter();
	static bool loadSave(CString fname); //Загрузить сохранение
	static bool saveGame(CString fname); //Сохранить игру
	static bool wasNewCommand(); //Была получена новая команда от игрока
	static void clearNewCommandFlag(); //очистить флаг новой команды
	static CString lastCommand(); //последняя команда от игрока
protected:
	static void startGameFile(CString gameFile, CString gameName, CString saveFile, int autolog);
private:
    static CString m_gameFile;
        static CString m_gameName;
	static CString m_lastCommand;
	static bool m_wasCommand;
};

