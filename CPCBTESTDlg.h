// CPCBTESTDlg.h : header file
//

#if !defined(AFX_CPCBTESTDLG_H__673E7EF4_4211_4178_900D_0D6F7CA9CE94__INCLUDED_)
#define AFX_CPCBTESTDLG_H__673E7EF4_4211_4178_900D_0D6F7CA9CE94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCPCBTESTDlg dialog

#include "ColorPickerCB.h"
#include "afxwin.h"

class CCPCBTESTDlg : public CDialog
{
// Construction
public:
	CCPCBTESTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCPCBTESTDlg)
	enum { IDD = IDD_CPCBTEST_DIALOG };
	CColorPickerCB	m_cbOutBack;
	CColorPickerCB	m_cbInBack;
	CColorPickerCB	m_cbInFont;
	CColorPickerCB	m_cbOutFont;
	int m_valueEdit;
	CFont m_font;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCPCBTESTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	void UpdateFontSize(int fontH);

	// Generated message map functions
	//{{AFX_MSG(CCPCBTESTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_editFontHeight;
	CStatic m_textExampleOut;
	CStatic m_textExampleIn;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEditSizeFont();
	afx_msg void OnCbnSelchangeFocusedcb();
	afx_msg void OnCbnSelchangeInactivecb();
	afx_msg void OnCbnSelchangeDisabledcb();
	afx_msg void OnCbnSelchangeDroppedcb();
	//afx_msg void OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	CButton m_CheckAutosay;
	CButton m_CheckSetFocusToOut;
	CComboBox m_ComboFontSize;
	afx_msg void OnCbnSelchangeComboFontSize();
	CButton mCheckAutoLog;
	CButton mCheckSoundList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPCBTESTDLG_H__673E7EF4_4211_4178_900D_0D6F7CA9CE94__INCLUDED_)
