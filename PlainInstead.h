// PlainInstead.h : главный файл заголовка для приложения PlainInstead
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// CPlainInsteadApp:
// О реализации данного класса см. PlainInstead.cpp
//

class CPlainInsteadApp : public CWinApp
{
public:
	CPlainInsteadApp();

private:
	BOOL m_openGame;
	CString currFilePath;
	CString currFileName;
	CString saveDir;
	CString saveGameNameDir;
	int soundBeforeMute;
	bool isMute;

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация
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
	afx_msg void OnNewGameFromFile();
	afx_msg void OnNewGameFromLib();
	afx_msg void OnRestartMenu();
	afx_msg void OnUpdateRestartMenu(CCmdUI *pCmdUI);
	afx_msg void OnEnterSetup();
	afx_msg void OnResetAllSettings();
	afx_msg void OnAppExit();
	afx_msg void OnVolumeDown();
	afx_msg void OnVolumeUp();
	afx_msg void OnVolumeOff();
	afx_msg void OnVolumeOn();
	afx_msg void OnUpdateVolumeUp(CCmdUI *pCmdUI);
	afx_msg void OnUpdateVolumeDown(CCmdUI *pCmdUI);
	afx_msg void OnUpdateVolumeOn(CCmdUI *pCmdUI);
	afx_msg void OnUpdateVolumeOff(CCmdUI *pCmdUI);
};

extern CPlainInsteadApp theApp;