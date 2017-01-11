#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
#include <set>


// диалоговое окно LauncherDialog

class LauncherDialog : public CDialog
{
	DECLARE_DYNAMIC(LauncherDialog)

public:
	LauncherDialog(CWnd* pParent = NULL);   // стандартный конструктор
	virtual ~LauncherDialog();
	bool isWantStartGame();
	CString getStartGamePath();
	CString getStartGameTitle();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAUNCHERDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV
	void showInstalledTabControls();
	void showNewTabControls();
	void CreateColumns();
	void RescanInstalled();
	void AddInstalledGame(CString name, bool checked, CString version, std::pair<CString, CString> path);
	void AddNewGame(CString name, bool checked, CString version, CString sz, CString page, std::pair<CString, CString> downloadPageAndInstallName);
	void SetCell(CListCtrl& ctrl, CString value, int nRow, int nCol);
	void UpdateNewGamesFromUrl(CString url);

	std::vector<CString> installedGamePath;
	std::set<CString> installedGameName;

	std::vector<std::pair<CString/*name*/, CString/*page*/> > networkGameDWPageAndName;
	std::set<CString> networkGameName;

	CString m_stGamePath;
	CString m_stGameTitle;
	bool    m_wantPlay;

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tab;
	virtual BOOL OnInitDialog();
	CListCtrl m_listInstalled;
	CListCtrl m_listNew;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTab1(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_btnDelete;
	CButton m_btnUpdate;
	CButton m_btnInstall;
	CButton m_btnOpenLink;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	afx_msg void OnBnClickedBtnDelGame();
	afx_msg void OnBnClickedBtnUpdate();
	afx_msg void OnBnClickedBtnOpenLink();
	afx_msg void OnBnClickedBtnInstall();
	afx_msg void OnBnClickedBtnPlayGamem();
	CButton m_btnPlayGame;
	CString currDir;
	afx_msg void OnBnClickedBtnResumeoldGame2();
	CButton m_btnResumeGame;
};
