// MainFrm.h : интерфейс класса CMainFrame
//


#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // создать только из сериализации
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Атрибуты
public:

// Операции
public:

// Переопределение
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Реализация
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // встроенные члены панели элементов управления
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Созданные функции схемы сообщений
protected:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};


