#pragma once
#include "afxwin.h"
#include <vector>
#include <map>


// ���������� ���� CSelectNewGameDialog

class CSelectNewGameDialog : public CDialog
{
	DECLARE_DYNAMIC(CSelectNewGameDialog)

public:
	CSelectNewGameDialog(CString& selGameFile, 
						CString& selName, 
						bool& selLastGame,
						CWnd* pParent = NULL);   // ����������� �����������
	BOOL OnInitDialog();
	virtual ~CSelectNewGameDialog();

// ������ ����������� ����
	enum { IDD = IDD_DIALOG_NEW_GAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListBox m_ListGames;
	CEdit m_GamesDescr;
	std::vector< std::pair<CString/*file name*/, CString/*file description*/> > fileNameDescr;
	afx_msg void OnLbnSelchangeListGames();
	afx_msg void OnBnClickedOk();
	CString& m_selGameFile;
	CString& m_selName;
	bool& m_bSelectLastGame;
	CString baseDir;
	afx_msg void OnBnClickedButtonStartLastGame();
};
