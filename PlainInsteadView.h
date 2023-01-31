// PlainInsteadView.h : интерфейс класса CPlainInsteadView
//


#pragma once
#include "afxwin.h"
#define ID_TIMER_1 100
#define ID_TIMER_2 101

#include <map>
#include "Wave.h"

class CPlainInsteadView : public CFormView
{
protected: // создать только из сериализации
	CPlainInsteadView();
	DECLARE_DYNCREATE(CPlainInsteadView)

public:
	enum{ IDD = IDD_PLAININSTEAD_FORM };
// Атрибуты
public:
	CPlainInsteadDoc* GetDocument() const;
	

// Операции
public:
	static CPlainInsteadView* GetCurrentView();

	void SetOutputText(CString newText, BOOL useHistory=TRUE);
	void UpdateSettings();
	void SendCommand(CString cmd);
	void InitFocusLogic();
	void UpdateFocusLogic();
// Переопределение
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV
	virtual void OnInitialUpdate(); // вызывается в первый раз после конструктора
	void ShowTextFromResource(LPCWSTR res_id); //Отобразить текст из ресурсов
	void UpdateFontSize();
	CFindReplaceDialog *m_pFindDialog;
	afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);
	bool FindStringInEdit(CString FindName, bool bMatchCase);
	bool wasFind;
	CString lastSearchStr;
	bool lastMatchCase;

// Реализация
public:
	virtual ~CPlainInsteadView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	static CPlainInsteadView* m_curView;
	BOOL m_auto_say;
	BOOL m_jump_to_out;
	//BOOL m_BeepList;
// Созданные функции схемы сообщений
protected:
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_OutEdit;
	CEdit m_InputEdit;
	CFont m_fontOut;
	CFont m_fontIn;
	int currInpHeight;
	COLORREF outBackCol;
	COLORREF outFontCol;
	COLORREF inBackCol;
	COLORREF inFontCol;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnSetfocusEditOut();
	afx_msg void OnTimer(UINT);		// функция реакции на таймер
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	bool useClipboard;
	afx_msg void OnFullHistory();
	afx_msg void OnBackHist();
	afx_msg void OnForwHist();
	afx_msg void OnManualStarter();
	afx_msg void OnManualCmdList();
	afx_msg void OnManualHowPlay();
	afx_msg void OnManualRuk1();
	afx_msg void OnManualRuk2();
	afx_msg void OnManualRuk3();
	afx_msg void OnManualRuk4();
	afx_msg void OnManualRuk5();
	afx_msg void OnManualRuk6();
	afx_msg void OnManualRuk7();
	afx_msg void OnManualBigWrap();
	afx_msg void OnFindText();
	afx_msg void OnFindNext();
	afx_msg void OnHistoryStop();
	afx_msg void OnHistoryStart();
int TryInsteadCommand(CString textIn, CString cmdForLog = L"", bool needSearchVariants=true,bool isFromEdit=false); //Отправка команды в интерпретатор
void updateText(char* txt=NULL);
void onNewInsteadCommand(char*cmd, char* p,CString cmdForLog = L"");
	void TurnOffLogging();
	CListBox mListScene;
	std::map<int,int> pos_id_scene;
//std::map<int/*list_pos*/, int/*code*/> act_on_scene;
		std::map<int, int> pos_id_ways;
	CListBox mListWays;
	CListBox mListInv;
	std::map<int, int> pos_id_inv;
	int inv_save_index=-1; //Индекс в map Сохраненного результата инвентаря
	afx_msg void OnUpdateOutView();
	afx_msg void OnStnClickedStaticScene();
	CStatic mStaticScene;
	CStatic mStaticInv;
	CStatic mStaticWays;
	afx_msg void OnLbnSetfocusListScene();
	afx_msg void OnLbnSetfocusListInv();
	afx_msg void OnLbnSetfocusListWays();
	afx_msg void OnGotoScene();
	afx_msg void OnGotoInv();
	afx_msg void OnGotoWays();
	CString baseSoundDir;
	Wave* wave_inv;
	Wave* wave_scene;
	Wave* wave_ways;
	afx_msg void OnMenuLog();
	afx_msg void OnMenuDebug();
	bool isLogOn;
	CString logFileName;
	CString savedSelInvM;
};
bool Utf8ToCString(CString& cstr, const char* utf8Str);
#ifndef _DEBUG  // отладочная версия в PlainInsteadView.cpp
inline CPlainInsteadDoc* CPlainInsteadView::GetDocument() const
   { return reinterpret_cast<CPlainInsteadDoc*>(m_pDocument); }
#endif

