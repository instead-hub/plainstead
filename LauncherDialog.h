#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
#include <unordered_set>
/*#include <set>
#include <map>*/
#include <unordered_map>
struct CString_hash
{
	size_t operator()(const CString& s) const noexcept
	{
		return std::hash<std::wstring_view>{}({ s.GetString(), (size_t)s.GetLength() });
	}
};
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

// ƒанные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAUNCHERDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV
	void showInstalledTabControls();
	void showNewTabControls();
	void CreateColumns();
	void RescanInstalled();
	void updateDataAfterInstalationOfGame(CString gameName, int sel, bool gameWasRemoved = false);
	void loadGame(CString gameName, CString gameDwnUrl, CString loadingMessage, CString afterLoadMessage, int sel);
	void AddNewGame(CString date,CString name,CString author,CString description, CString version, CString language, std::pair<CString, CString> downloadPageAndInstallName, CListCtrl& games, CString sz = L"", CString page = L"");
	void SetCell(CListCtrl& ctrl, CString value, int nRow, int nCol);
	bool UpdateGamesFromUrl(CString url, CString res_path, bool updateInstalledGames = true);
	void ReadNewGamesFromXMLAndAdd(CString temp_xmlfile, bool updateInstalledGames=true);
	void updateAllGames(bool updateInstalledGames=true);
	void ClearNewList();
	void SortColumn(CListCtrl* ctrl, int columnIndex, bool ascending);

	//std::unordered_map<CString/*game name*/, CString/*game path*/> installedGamePath;
	std::unordered_set<CString,CString_hash> installedGameNameCache; //кеш дл€ быстрой проверки

	//std::vector<std::pair<CString/*name*/, CString/*page*/> > networkGameDWPageAndName;
	//std::set<CString> networkGameName;

	std::unordered_map<CString/*game name*/, std::pair<CString /*accessibility comment*/, int /*accessibility status*/>, CString_hash> approveInfo;
	CString m_stGamePath;
	CString m_stGameTitle;
	bool    m_wantPlay;
	int     m_lastSelFilter;
	bool    m_sortInstalledUp;
	int     m_sortInstalledLastItem;
	
	bool    m_sortNewUp;
	int     m_sortNewLastItem;

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
	CComboBox m_comboFiler;
	afx_msg void OnCbnSelchangeComboFilter();
	afx_msg void OnHdnItemclickListInstalled(NMHDR *pNMHDR, LRESULT *pResult);
private:
};
