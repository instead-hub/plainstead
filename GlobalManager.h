#pragma once

#include <vector>

class GlobalManager
{
public:
	static GlobalManager& getInstance() {
        static GlobalManager instance;
        return instance;
    }
	void userStartGame(); //������������ ����� ����
	void userSavedFile();  //������������ �������� ����
	void userNewCommand(); //���������� ����� ������� �������������
	bool isUserSaveLastFile(); //������� ����, ��� ������������ �������� ����
	bool isUserStartGame();
	bool isEmptyCmd();
	void setEmptyCmd(bool emptyCmd);
	void appendCommandAndRespond(CString command, CString respond); //���������� ������� � ��������
	void appendCommand(CString command); //�������� ����� �������
	void appendLastRespond(CString command); //�������� ��������� ����� �� �������
	CString fullHistoryData(); //��� ������� ������� (�������+�����)
	CString previosHistoryData(); //���������� ������� � ������� (�������+�����)
	bool previosHistoryHave(); //������� ����������� ��������
	CString nextHistoryData(); //��������� ������� � ������� (�������+�����)
	bool nextHistoryHave(); //������� ���������� ��������
	void enableHistory(bool isEn); //��������� ������ �������

	CString commandData(); //������� � �������
	bool previosCommandMove(); //������� ���������� �������
	bool nextCommandMove(); //������� ��������� �������

	bool isIgnoreExitDialog; //������������ ������ �� ������
	bool isUseMenu(); //������������ �� ����
	bool isAutoMenuDetect(); //�������������� ����������� ����
	CString keyMenuString(); //�������� ������ ��� ����������� ����
	void setUseMenu();
	static int lastString;
private:
	GlobalManager();
	~GlobalManager();
	GlobalManager( const GlobalManager&);  
    GlobalManager& operator=( GlobalManager& );

	bool m_haveFileSaved;
	bool m_userStartGame;
	bool emptyCmd = true; //���� ������������ �������� ���������� �� stead2,��� ����� ������ ���� �� stead2,��� ����� ������ ������ api,� ������� stead3.
	std::vector< std::pair<CString/*command*/,CString/*respond*/> > commandHistory; //������� ������
	int m_currHistPos; //������� ��������� � �������
	int m_currCmdPos; //������� ��������� � ��������
	bool m_useMenu;
	bool m_en_history;
};
