// PlainInstead.h : ������� ���� ��������� ��� ���������� PlainInstead
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"       // �������� �������


// CPlainInsteadApp:
// � ���������� ������� ������ ��. PlainInstead.cpp
//

class CPlainInsteadApp : public CWinApp
{
public:
	CPlainInsteadApp();

private:
	BOOL m_openGame;
	CString currFilePath;
	CString currFileName;
	bool isMusicMute;
	BOOL isSoundsMute;
int musicVolumeBeforeMute;
int soundsVolumeBeforeMute;
	int effectsBeforeMute;
	bool isMuteEffects;

// ���������������
public:
	virtual BOOL InitInstance();

// ����������
	void StartNewGameFile(CString file,CString name);
	void OnFileOpen();
	void OnFileSave();
    //void OnFileNewGame();
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveGame(CCmdUI *pCmdUI);
	afx_msg void OnViewFullHist();
	afx_msg void OnViewPrevHist();
	afx_msg void OnViewNextHist();
	afx_msg void OnUpdateViewFullHist(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewPrevHist(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewNextHist(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDebug(CCmdUI* pCmdUI);
		afx_msg void OnNewGameFromFile();
	afx_msg void OnNewGameFromLib();
	afx_msg void OnRestartMenu();
	afx_msg void OnUpdateRestartMenu(CCmdUI *pCmdUI);
	afx_msg void OnEnterSetup();
	afx_msg void OnResetAllSettings();
	afx_msg void OnAppExit();
	afx_msg void OnVolumeDown();
	afx_msg void OnVolumeUp();
	afx_msg void OnVolumeOn();
		afx_msg void OnUpdateVolumeUp(CCmdUI *pCmdUI);
	afx_msg void OnUpdateVolumeDown(CCmdUI *pCmdUI);
	afx_msg void OnUpdateVolumeOn(CCmdUI *pCmdUI);
	afx_msg void OnSoundsVolumeDown();
	afx_msg void OnSoundsVolumeUp();
	afx_msg void OnSoundsVolumeOn();
	afx_msg void OnUpdateSoundsVolumeUp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSoundsVolumeDown(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSoundsVolumeOn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSoundsVolumeOff(CCmdUI* pCmdUI);
	afx_msg void OnAddGameToLib();
	afx_msg void OnOpenManager();
	afx_msg void OnListsndOn();
	afx_msg void OnListsndDown();
	afx_msg void OnListsndUp();
	afx_msg void OnUpdateListsndOn(CCmdUI *pCmdUI);
	afx_msg void OnUpdateListsndDown(CCmdUI *pCmdUI);
	afx_msg void OnUpdateListsndUp(CCmdUI *pCmdUI);
};

extern CPlainInsteadApp theApp;