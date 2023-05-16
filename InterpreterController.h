#pragma once

class InterpreterController
{
public:
    InterpreterController(void);
    ~InterpreterController(void);
	static void startGameFile(CString gameFile, CString gameName, CString saveFile = L"", int autolog=0, boolean reload = false);
	static void startGameFile(CString gameFile, CString gameName, int autolog = 0, boolean reload = false);
    static CString RunInterpreter(CString command); //������ ��������������, � ������������ ����� � �������� �� ���������
	static void endInterpreter();
	static bool loadSave(CString fname, boolean reload = false); //��������� ����������
	static bool saveGame(CString fname); //��������� ����
	static bool wasNewCommand(); //���� �������� ����� ������� �� ������
	static void clearNewCommandFlag(); //�������� ���� ����� �������
	static CString lastCommand(); //��������� ������� �� ������
protected:
private:
    static CString m_gameFile;
	static void InterpreterController::loadFile(CString gameFile, CString gameName);
        static CString m_gameName;
	static CString m_lastCommand;
	static bool m_wasCommand;
};

