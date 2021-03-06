#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
#include <set>
#include <map>


// ���������� ���� LauncherDialog

class LauncherDialog : public CDialog
{
	DECLARE_DYNAMIC(LauncherDialog)

public:
	LauncherDialog(CWnd* pParent = NULL);   // ����������� �����������
	virtual ~LauncherDialog();
	bool isWantStartGame();
	CString getStartGamePath();
	CString getStartGameTitle();

// ������ ����������� ����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAUNCHERDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV
	void showInstalledTabControls();
	void showNewTabControls();
	void CreateColumns();
	void RescanInstalled();
	void AddInstalledGame(CString name, CString version, std::pair<CString, CString> path);
	void AddNewGame(CString name, CString version, CString sz, CString page, std::pair<CString, CString> downloadPageAndInstallName);
	void SetCell(CListCtrl& ctrl, CString value, int nRow, int nCol);
	void UpdateNewGamesFromUrl(CString url, CString temp_xmlfile, bool is_sander = false);
	void ReadNewGamesFromXMLAndAdd(CString temp_xmlfile, bool is_sander);
	void UpdateNewGamesRssAdditionalInfoFromUrl(CString url, CString temp_xmlfile);
	void ReadAdditionalInfoFromXMLRss(CString temp_xmlfile);
	bool UpdateApprovedGamesFromUrl(CString url, CString res_path);
	void UpdateApprovedFromFile();
	void ClearNewList();
	void SortColumn(CListCtrl* ctrl, int columnIndex, bool ascending);

	//std::map<CString/*game name*/, CString/*game path*/> installedGamePath;
	std::set<CString> installedGameNameCache; //��� ��� ������� ��������

	//std::vector<std::pair<CString/*name*/, CString/*page*/> > networkGameDWPageAndName;
	//std::set<CString> networkGameName;

	std::map<CString/*game name*/, std::pair<CString /*approve*/, CString /*info*/> > approveInfo;
	std::map<CString/*game title*/, std::pair<CString /*pubDate*/, CString /*Desc*/> > rssInfo;

	CString m_stGamePath;
	CString m_stGameTitle;
	CString m_gameBaseDir;
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
	CButton m_CheckSander;
};
