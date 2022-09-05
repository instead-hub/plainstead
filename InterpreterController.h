#pragma once

class InterpreterController
{
public:
    InterpreterController(void);
    ~InterpreterController(void);
    static void startGameFile(CString gameFile, CString gameName, int autolog);//���������� ������� ����
    static CString RunInterpreter(CString command); //������ ��������������, � ������������ ����� � �������� �� ���������
	static void endInterpreter();
	static bool loadSave(CString fname); //��������� ����������
	static bool saveGame(CString fname); //��������� ����
	static bool wasNewCommand(); //���� �������� ����� ������� �� ������
	static void clearNewCommandFlag(); //�������� ���� ����� �������
	static CString lastCommand(); //��������� ������� �� ������
protected:
	static void startGameFile(CString gameFile, CString gameName, CString saveFile, int autolog);
private:
    static CString m_gameFile;
        static CString m_gameName;
	static CString m_lastCommand;
	static bool m_wasCommand;
};

