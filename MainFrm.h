// MainFrm.h : ��������� ������ CMainFrame
//


#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // ������� ������ �� ������������
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ��������
public:

// ��������
public:

// ���������������
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ����������
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ���������� ����� ������ ��������� ����������
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// ��������� ������� ����� ���������
protected:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};


