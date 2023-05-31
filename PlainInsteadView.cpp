// PlainInsteadView.cpp : реализация класса CPlainInsteadView
//
#include "stdafx.h"

#include "PlainInstead.h"

#include "PlainInsteadDoc.h"
#include "PlainInsteadView.h"

#include "InterpreterController.h"
#include "MultiSpeech.h"
#include "GlobalManager.h"
#include "IniFile.h"
#include "global.h"
#include <regex>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" {
#include "instead\instead.h"
	extern char temp_buff[5000];
	extern BOOL WasReadLine;
	extern BOOL WasReadScreen;
	extern char line_buff[1000]; //буфер ввода пользователя
	extern BOOL AskSaveLoadTadsGame; //запрос на сохранение/загрузку игры
	extern BOOL IsNeedSaveTadsGame; //Необходимо сохранение игры
	extern BOOL IsOkSaveLoadTadsGame; //Успешность сохранения/загрузки
	extern char TadsFileName[200];
}

#define txt text_er + text[debug] + ways_er +(debug?ways:L"") + inv_er + (debug ? inv : L"")
static BOOL m_auto_say;
CPlainInsteadView* CPlainInsteadView::m_curView = 0;
CEdit* updateEdit = NULL;
void CALLBACK EXPORT OnTimerUpdateText(HWND, UINT, UINT, DWORD);

CPlainInsteadView* CPlainInsteadView::GetCurrentView()
{
	return m_curView;
}

// CPlainInsteadView

static UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);
static CString text[2];
static CString first_er, text_er, inv_er, ways_er, inv, ways;
static bool debug;
IMPLEMENT_DYNCREATE(CPlainInsteadView, CFormView)

BEGIN_MESSAGE_MAP(CPlainInsteadView, CFormView)
	ON_WM_SIZE()
	ON_EN_SETFOCUS(IDC_EDIT_OUT, &CPlainInsteadView::OnEnSetfocusEditOut)
	ON_WM_TIMER()		// реагировать на таймер
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_COMMAND(ID_FULL_HISTORY, &CPlainInsteadView::OnFullHistory)
	ON_COMMAND(ID_BACK_HIST, &CPlainInsteadView::OnBackHist)
	ON_COMMAND(ID_FORW_HIST, &CPlainInsteadView::OnForwHist)
	ON_COMMAND(ID_MANUAL_STARTER, &CPlainInsteadView::OnManualStarter)
	ON_COMMAND(ID_MANUAL_CMD_LIST, &CPlainInsteadView::OnManualCmdList)
	ON_COMMAND(ID_MANUAL_HOW_PLAY, &CPlainInsteadView::OnManualHowPlay)
	ON_COMMAND(ID_MANUAL_RUK1, &CPlainInsteadView::OnManualRuk1)
	ON_COMMAND(ID_MANUAL_RUK2, &CPlainInsteadView::OnManualRuk2)
	ON_COMMAND(ID_MANUAL_RUK3, &CPlainInsteadView::OnManualRuk3)
	ON_COMMAND(ID_MANUAL_RUK4, &CPlainInsteadView::OnManualRuk4)
	ON_COMMAND(ID_MANUAL_RUK5, &CPlainInsteadView::OnManualRuk5)
	ON_COMMAND(ID_MANUAL_RUK6, &CPlainInsteadView::OnManualRuk6)
	ON_COMMAND(ID_MANUAL_RUK7, &CPlainInsteadView::OnManualRuk7)
	ON_COMMAND(ID_MANUAL_BIG_WRAP, &CPlainInsteadView::OnManualBigWrap)
	ON_COMMAND(ID_FIND_TEXT, &CPlainInsteadView::OnFindText)
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace)
	ON_COMMAND(ID_FIND_NEXT, &CPlainInsteadView::OnFindNext)
	ON_COMMAND(ID_HISTORY_STOP, &CPlainInsteadView::OnHistoryStop)
	ON_COMMAND(ID_HISTORY_START, &CPlainInsteadView::OnHistoryStart)
	ON_COMMAND(ID_UPDATE, &CPlainInsteadView::OnUpdateOutView)
	ON_STN_CLICKED(IDC_STATIC_SCENE, &CPlainInsteadView::OnStnClickedStaticScene)
	ON_LBN_SETFOCUS(IDC_LIST_SCENE, &CPlainInsteadView::OnLbnSetfocusListScene)
	ON_LBN_SETFOCUS(IDC_LIST_INV, &CPlainInsteadView::OnLbnSetfocusListInv)
	ON_LBN_SETFOCUS(IDC_LIST_WAYS, &CPlainInsteadView::OnLbnSetfocusListWays)
	ON_COMMAND(ID_GOTO_SCENE, &CPlainInsteadView::OnGotoScene)
	ON_COMMAND(ID_GOTO_INV, &CPlainInsteadView::OnGotoInv)
	ON_COMMAND(ID_GOTO_WAYS, &CPlainInsteadView::OnGotoWays)
	ON_COMMAND(ID_MENU_LOG, &CPlainInsteadView::OnMenuLog)
	ON_COMMAND(ID_MENU_DEBUG, &CPlainInsteadView::OnMenuDebug)
END_MESSAGE_MAP()

// создание/уничтожение CPlainInsteadView

CPlainInsteadView::CPlainInsteadView()
	: CFormView(CPlainInsteadView::IDD)
{
	EnableActiveAccessibility();
	// TODO: добавьте код создания
	outFontCol = RGB(0, 0, 0);
	outBackCol = RGB(240, 240, 240);
	inFontCol = RGB(0, 0, 0);
	inBackCol = RGB(255, 255, 255);
	currInpHeight = 20;
	useClipboard = false;
	m_auto_say = true;
	m_jump_to_out = false;
	m_pFindDialog = NULL;
	wasFind = false;
	wave_inv = 0;
	wave_ways = 0;
	wave_scene = 0;
	isLogOn = false;
}

CPlainInsteadView::~CPlainInsteadView()
{
	if (wave_inv) delete wave_inv;
	if (wave_ways) delete wave_ways;
	if (wave_scene) delete wave_scene;
}

void CPlainInsteadView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_OUT, m_OutEdit);
	DDX_Control(pDX, IDC_EDIT_INP, m_InputEdit);
	DDX_Control(pDX, IDC_LIST_SCENE, mListScene);
	DDX_Control(pDX, IDC_LIST_INV, mListInv);
	DDX_Control(pDX, IDC_LIST_WAYS, mListWays);
	DDX_Control(pDX, IDC_STATIC_SCENE, mStaticScene);
	DDX_Control(pDX, IDC_STATIC_INV, mStaticInv);
	DDX_Control(pDX, IDC_STATIC_WAYS, mStaticWays);
}

BOOL CPlainInsteadView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: изменить класс Window или стили посредством изменения
	//  CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CPlainInsteadView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_curView = this;

	m_OutEdit.SetWindowTextW(
		L"Краткая справка:\r\n"
		L"Для начала новой игры из библиотеки нажмите CTRL+M и выберите игру и нажмите ENTER.\r\n"
		L"Если вы скачали игру, которой нет в библиотеке, то нажмите CTRL+N и выберите архив с игрой.\r\n"
		L"Для того чтобы играть выберите пункт в одном из списков - объекты, инвентарь, пути и нажмите клавишу ENTER.\r\n"
		L"Когда вы находитесь в инвентаре, вам необходимо сначала выбрать пункт, нажать ENTER, а затем выбрать второй пункт и нажать ENTER.\r\n"
		L"Поле ввода служит для ввода текста в метапарсерных играх (только instead 3),или для ввода команд,если в этом есть необходимость. В метапарсерных играх,скорее всего,взаимодействие с инвентарём не будет работать.\r\n"
		L"Если при взаимодействием с объектами или при вводе текста ничего не изменилось,значит,скорее всего,текст остался тем же,какой был перед взаимодействием с объектами или ввода команды.\r\n"
		L"Сохранение и загрузка игр происходит обязательно в каталоге с текущей игрой (иногда в подкаталоге autosaves,если игра сама сохраняет своё состояние), менять на другой нельзя.\r\n"
		L"При сохранении, указывайте пожалуйста имя файла латинскими буквами или цифрами.\r\n"
		L"Внимание! Большие архивы могут не распаковываться программой через менеджер или установку в библиотеку. Попробуйте самостоятельно распаковать их в папку с играми."
	);

	//Инициализация голосового ввода
	MultiSpeech::getInstance();

	//Обновление настроек
	UpdateSettings();
	//Запуск таймера обновления текста
	//SetTimer(ID_TIMER_2,100,OnTimerUpdateText);
	//чтение настроек
	CIniFile mainSettings;
	useClipboard = mainSettings.GetInt(L"main", L"useClipboard", 0);

	updateEdit = &m_OutEdit;
}


// диагностика CPlainInsteadView

#ifdef _DEBUG
void CPlainInsteadView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPlainInsteadView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CPlainInsteadDoc* CPlainInsteadView::GetDocument() const // встроена неотлаженная версия
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlainInsteadDoc)));
	return (CPlainInsteadDoc*)m_pDocument;
}
#endif //_DEBUG


// обработчики сообщений CPlainInsteadView

void CPlainInsteadView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: добавьте свой код обработчика сообщений
	// Автомасштабирование компонентов	
	if (m_InputEdit.m_hWnd && m_InputEdit.IsWindowVisible())
	{
		m_InputEdit.ShowWindow(SW_HIDE);
		m_InputEdit.EnableWindow(FALSE);
	}
	int h_static = 30;
	int dh_static_text = 15;
	int h_static_text = h_static - dh_static_text;
	int sz_list = 200;
	int h_list = (cy / 3 - h_static);
	if (m_OutEdit.m_hWnd) m_OutEdit.SetWindowPos(NULL, 0, 0, cx - sz_list, cy - h_static, SWP_NOACTIVATE | SWP_NOZORDER);
	if (m_InputEdit.m_hWnd && !m_InputEdit.IsWindowVisible())
	{
		m_InputEdit.ShowWindow(SW_SHOW);
		m_InputEdit.EnableWindow(TRUE);
		m_InputEdit.SetWindowPos(NULL, 0, cy - h_static, cx - sz_list, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
	//if (mStaticScene.m_hWnd) mStaticScene.SetWindowPos(NULL, 2+cx - sz_list, dh_static_text, sz_list, h_static_text, SWP_NOACTIVATE | SWP_NOZORDER);
	if (mListScene.m_hWnd) mListScene.SetWindowPos(NULL, 2 + cx - sz_list, h_static, sz_list, h_list, SWP_NOACTIVATE | SWP_NOZORDER);

	//if (mStaticInv.m_hWnd) mStaticInv.SetWindowPos(NULL, 2 + cx - sz_list, h_static +h_list + dh_static_text, sz_list, h_static_text, SWP_NOACTIVATE | SWP_NOZORDER);
	if (mListInv.m_hWnd)   mListInv.SetWindowPos(NULL, 2 + cx - sz_list, h_static + h_list + h_static, sz_list, h_list, SWP_NOACTIVATE | SWP_NOZORDER);

	//if (mStaticWays.m_hWnd) mStaticWays.SetWindowPos(NULL, 2 + cx - sz_list, h_static + h_list+ h_static + h_list+ dh_static_text, sz_list, h_static_text, SWP_NOACTIVATE | SWP_NOZORDER);
	if (mListWays.m_hWnd)  mListWays.SetWindowPos(NULL, 2 + cx - sz_list, h_static + h_list + h_static + h_list + h_static, sz_list, h_list, SWP_NOACTIVATE | SWP_NOZORDER);
}

bool Utf8ToCString(CString& cstr, const char* utf8Str)
{
	size_t utf8StrLen = strlen(utf8Str);

	if (utf8StrLen == 0)
	{
		cstr.Empty();
		return true;
	}

	LPWSTR ptr = cstr.GetBuffer(utf8StrLen + 1);

	// CString is UNICODE string so we decode
	int newLen = MultiByteToWideChar(
		CP_UTF8, 0,
		utf8Str, utf8StrLen, ptr, utf8StrLen + 1);
	if (!newLen)
	{
		cstr.ReleaseBuffer(0);
		return false;
	}

	cstr.ReleaseBuffer(newLen);
	return true;
}

static std::string utf8_encode(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}
static CString getError(CString cmd) {
	const char* er = instead_err();
	if (er) {
		CString error;
		//Конвертируем char* в строку.
		Utf8ToCString(error, er);
		if (!cmd) cmd = ""; else if (cmd.GetLength() > 0)cmd = L"\n" + cmd + L"\n";
		instead_err_msg(NULL);
		return cmd + error + L"\n";
	}
	return L"";
}
static CString getError() {
	return getError(NULL);
}
//Ядро движка-оболочки
//Обработка инстеад-текста и превращение в меню
static std::wstring process_instead_text(std::wstring inp, //входной текст
	CListBox& resBox, //Поле для добавления элементов
	std::map<int, int>& prev_map, //Предыдущий мап
	std::map<int/*list_pos*/, int/*id_obj*/>& map_action, //Карта соответствий поля с id-объекта
	bool& shouldRedraw, //Поменялся ли список.
	bool append_num = false, //добавлять номер в список (для отладки)
	bool clear = false //Очищать ли элементы массива,перед добавлением туда текста
) {
	//Обработка отображения объектов сцены
	//const std::wregex regex(L"\\{\\s*(\\w+)\\s*\\#(\\d+)\\}"); //Для фигурных скобочек
	//const std::wregex regex(L"(\\w+)\\s*\\((\\d+)\\)"); //Для круглых после
	const std::wregex regex(L"\\[a\\]([^]*?)\\#(-?\\d+)\\[/a\\]", std::regex_constants::optimize); //для тэгов [a]
	std::wsmatch match;
	std::wsregex_iterator next(inp.begin(), inp.end(), regex);
	std::wsregex_iterator end;
	int pos = 0;
	int sel = resBox.GetCurSel();
	resBox.SetRedraw(false);
	while (next != end) {
		match = *next;
		if (match.size() == 3)
		{
			CString addStr(match[1].str().data()); //Имя строки, меню
			addStr = addStr.Trim(); //триммируем
			CString numStr(match[2].str().data()); //id-объекта для реакции
			int obj_id = _wtoi(numStr); //id-объекта
			if (append_num) addStr = addStr + L"(" + numStr + L")";
			map_action.insert(std::make_pair(pos, obj_id));
			CString oldText;
			if (pos <= resBox.GetCount() - 1)resBox.GetText(pos, oldText);
			if (!prev_map.count(pos) || (map_action[pos] != prev_map[pos] && map_action[pos] != -prev_map[pos]) || oldText != addStr) {
				//if(prev_map.count(pos) &&map_action[pos] != prev_map[pos])AfxMessageBox(L"Тест");
				shouldRedraw = true;
				/*CString test;
				test.Format(L"%s %d %d", addStr,pos,resBox.GetCount());
					AfxMessageBox(test);*/
				resBox.DeleteString(pos);
				resBox.InsertString(pos, addStr);
				if (sel == pos) sel = 0;
			}
			pos++;
		}
		next++;
	}
	while (pos < resBox.GetCount()) {
		if (!shouldRedraw) shouldRedraw = true;
		map_action.erase(resBox.GetCount() - 1);
		resBox.DeleteString(resBox.GetCount() - 1);
	}
	resBox.SetRedraw(true);
	if (resBox.GetCount() == 0) pos = 0;
	if (pos == 0) resBox.ResetContent();
	else if (resBox.GetCurSel() < 0 || resBox.GetCurSel() > pos - 1) {
		/*CString a;
		a.Format(L"%d %d",resBox.GetCurSel(),pos-1);
		AfxMessageBox(a);*/
		resBox.SetCurSel(sel<0 ? 0 : sel>pos - 1 ? pos - 1 : sel);
	}
	std::wstring result;
	std::regex_replace(std::back_inserter(result), inp.begin(), inp.end(), regex, L"$1");
	return result;
}
static CString getLogsDir() {
	TCHAR buff[MAX_PATH];
	::GetModuleFileName(NULL, buff, std::size(buff));
	CString baseDir = buff;
	/*free(buff);
	delete[] buff;*/
	baseDir = baseDir.Left(baseDir.ReverseFind(_T('\\')) + 1);
	return baseDir + L"logs\\";
}
void createLogsDirIfNeedAndGetFileName(char* buff) {
	CString logsDir = getLogsDir();
	if (GetFileAttributes(logsDir) == INVALID_FILE_ATTRIBUTES) SHCreateDirectoryEx(NULL, logsDir, NULL);
	logsDir += "debug.txt";
	strcpy(buff, CT2A(logsDir));
}
void CPlainInsteadView::onNewInsteadCommand(char* cmd, char* p, CString cmdForLog, int rc) {
	if (inv_save_index >= 0) //Снимаем выбор и восстанавливаем item
	{
		int pos = mListInv.GetCurSel();
		mListInv.SetDlgItemTextW(inv_save_index, savedSelInvM);
		mListInv.DeleteString(inv_save_index);
		mListInv.InsertString(inv_save_index, savedSelInvM);
		mListInv.SetCurSel(pos);
		inv_save_index = -1;
	}
	bool shouldRedraw = false;
	CString er, tmp;
	Utf8ToCString(tmp, cmd);
	//free(cmd);
	std::map<int, int> prev_map = pos_id_scene;
	short onlyInvRedraw = 1; //Если -1 - перисовываем всё,если 0 - всё,кроме текста,если 1 - ничего.
	CString prev_er = text_er;
	if (!first_er.IsEmpty()) {
		text_er = first_er;
		first_er = L"";
	}
	else text_er = L"";
	text_er += getError(tmp);
	if (rc || (p && *p) || (text_er && text_er.GetLength() > 0)) {
		//act_on_scene.clear();
		if (p && *p) {
			Utf8ToCString(tmp, p);
			free(p);
			if (text[1] != tmp) {
				pos_id_scene.clear();
				text[1] = tmp;
				std::wstring buf = tmp.GetBuffer();
				std::wstring result = process_instead_text(buf, mListScene, prev_map, pos_id_scene, shouldRedraw);
				tmp.ReleaseBuffer();
				Utf8ToCString(tmp, cmd);
				text[0] = result.data();
				onlyInvRedraw = -1;
				if (/*m_BeepList && */shouldRedraw) {
					shouldRedraw = false;
					wave_scene->play();
				}
			}
		}
		else {
			pos_id_scene.clear();
			mListScene.ResetContent();
			text[0] = L"";
			text[1] = L"";
			onlyInvRedraw = -1;
		}
		if (text_er != prev_er) onlyInvRedraw = -1; //Если ошибка изменилась,также обновляем текст.
		p = instead_cmd("way", NULL);
		prev_er = ways_er;
		ways_er = getError(L"way");
		if (p && *p) {
			Utf8ToCString(tmp, p);
			free(p);
			//Добавление путей к окну вывода
//text[0].Append(L">> ");
//text[0].Append(tmp);
//text[0].Append(L"\n");
		//Добавляем пути для отладочной переменной,если они изменились.
			if (tmp != ways) {
				if (onlyInvRedraw != -1)onlyInvRedraw = 0;
				ways = tmp;
				prev_map = pos_id_ways;
				pos_id_ways.clear();
				std::wstring buf = tmp.GetBuffer();
				std::wstring result = process_instead_text(buf, mListWays, prev_map, pos_id_ways, shouldRedraw);
				tmp.ReleaseBuffer();
				if (/*m_BeepList && */shouldRedraw) {
					shouldRedraw = false;
					wave_ways->play();
				}
			}
		}
		else {
			pos_id_ways.clear();
			mListWays.ResetContent();
			ways = L"";
			//ways_er = L"";
			if (onlyInvRedraw != -1)onlyInvRedraw = 0;
		}
	}
	if (ways_er != prev_er) onlyInvRedraw = -1; //Раз у нас возникла ошибка,обновляем текст.
	//Если результат команды NULL,перерисовываем только инвентарь.
	p = instead_cmd("inv", NULL);
	prev_er = inv_er;
	inv_er = getError(L"inv");
	if (p && *p) {
		Utf8ToCString(tmp, p);
		free(p);
		//Добавление инвентаря к окну вывода
		//text[0].Append(L"** ");
		//text[0].Append(tmp);
		//text[0].Append(L"\n");
		//Добавляем инвентарь для отладочной переменной,если инвентарь изменился.
		if (tmp != inv) {
			if (onlyInvRedraw != -1)onlyInvRedraw = 0;
			prev_map = pos_id_inv;
			pos_id_inv.clear();
			std::wstring buf = tmp.GetBuffer();
			std::wstring result = process_instead_text(buf, mListInv, prev_map, pos_id_inv, shouldRedraw);
			tmp.ReleaseBuffer();
			inv = tmp;
			if (/*m_BeepList && */shouldRedraw) {
				//PlaySound(baseSoundDir+_T("inventory.wav"), NULL, SND_MEMORY | SND_FILENAME | SND_ASYNC | SND_NOSTOP);
				shouldRedraw = false;
				wave_inv->play();
			}
		}
	}
	else {
		pos_id_inv.clear();
		mListInv.ResetContent();
		inv = L"";
		//inv_er = L"";
		if (onlyInvRedraw != -1)onlyInvRedraw = 0;
	}
	if (inv_er != prev_er) onlyInvRedraw = -1; //Раз у нас возникла ошибка,обновляем текст.
	if (onlyInvRedraw == -1 || onlyInvRedraw == 0 && debug) {
		if (onlyInvRedraw == -1) {
			text[0].Replace(L"\n", L"\r\n");
		}
		updateText();
	}
	if (isLogOn)
	{
		CString logsDir = getLogsDir();
		if (GetFileAttributes(logsDir) == INVALID_FILE_ATTRIBUTES) SHCreateDirectoryEx(NULL, logsDir, NULL);
		FILE* fStream;
		//errno = 0;
		int result = _tfopen_s(&fStream, logFileName, L"at,ccs=UTF-8");
		if (result)
		{
			AfxMessageBox(L"Не могу записать файл лога! Логирование отключаю.");
			TurnOffLogging();
			return;
		}
		CStdioFile flog(fStream);
		flog.SeekToEnd();
		flog.WriteString(L"\n\n>" + cmdForLog + L"\n");
		flog.SeekToEnd();
		flog.WriteString(text[1]);
		flog.SeekToEnd();
		if (mListScene.GetCount() > 0)
		{
			flog.WriteString(L"\nСцена: ");
			flog.SeekToEnd();
			CString itm;
			for (int i = 0; i < mListScene.GetCount(); i++) {
				mListScene.GetText(i, itm);
				flog.WriteString(itm + L"; ");
				flog.SeekToEnd();
			}
		}
		if (mListInv.GetCount() > 0)
		{
			flog.WriteString(L"\nИнвентарь: ");
			flog.SeekToEnd();
			CString itm;
			for (int i = 0; i < mListInv.GetCount(); i++) {
				mListInv.GetText(i, itm);
				flog.WriteString(itm + L"; ");
				flog.SeekToEnd();
			}
		}
		if (mListWays.GetCount() > 0)
		{
			flog.WriteString(L"\nПути: ");
			flog.SeekToEnd();
			CString itm;
			for (int i = 0; i < mListWays.GetCount(); i++) {
				mListWays.GetText(i, itm);
				flog.WriteString(itm + L"; ");
				flog.SeekToEnd();
			}
		}
		flog.Close();
	}
}
static void speak(CString result) {
	if (m_auto_say) MultiSpeech::getInstance().Say(result);
}
void CPlainInsteadView::updateText(char* Text) {
	CString result;
	if (Text) {
		Utf8ToCString(result, Text);
		//free(txt);
		result.ReleaseBuffer(result.GetLength());
	}
	else result = txt;
	m_OutEdit.SetWindowTextW(result);
	if (!m_jump_to_out) UpdateFocusLogic();
	if (m_jump_to_out && GetFocus() != &m_OutEdit) m_OutEdit.SetFocus();
	speak(result);
}

static std::string utf8_encode(const CStringW srcW)
{
	std::string utf8;

	if (!srcW.IsEmpty())
	{
		auto size = ::WideCharToMultiByte(CP_UTF8, 0, srcW.GetString(), srcW.GetLength(), 0, 0, 0, 0);
		utf8.resize((size_t)size, {});
		::WideCharToMultiByte(CP_UTF8, 0, srcW.GetString(), srcW.GetLength(), utf8.data(), utf8.size(), 0, 0);
	}
	return utf8;
}

static CStringW utf8_to_utf16(const std::string_view utf8)
{
	CStringW utf16;
	if (!utf8.empty() && utf8.size() <= (size_t)(std::numeric_limits<int>::max)())
	{
		if (const auto size = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), 0, 0); size > 0)
		{
			utf16 = CStringW{ L'\0',size };
			::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), utf16.GetBuffer(), utf16.GetLength());
			utf16.ReleaseBuffer();
		}
	}
	return utf16;
}
int CPlainInsteadView::TryInsteadCommand(CString textIn, CString cmdForLog, bool needSearchVariants, short isFromEdit)
{
	CString resout;
	int rc;
	char cmd[512];
	char* p;
	std::string newCmd = utf8_encode(textIn);
	CString er = L"";
	bool is_saving_or_loading = false;
	if (GlobalManager::getInstance().isEmptyCmd()) {
		first_er = getError(!GlobalManager::getInstance().isUserRestartGame() ? L"start" : L"load");
	}
	if (textIn.Find(L"save ") == 0 || textIn.Find(L"load ") == 0)
	{
		GlobalManager::getInstance().userSavedFile();
		is_saving_or_loading = true;
	}
	if (!is_saving_or_loading && (!textIn.IsEmpty() && textIn != "look" || isFromEdit)) GlobalManager::getInstance().userNewCommand();
	//Обработка строки в Instead
	if (needSearchVariants) {
		snprintf(cmd, sizeof(cmd), "use %s", newCmd.data());
		p = instead_cmd(cmd, &rc);
		if (rc) { /* try go */
			free(p);
			snprintf(cmd, sizeof(cmd), "go %s", newCmd.data());
			p = instead_cmd(cmd, &rc);
		}
		if (rc) free(p);
	}
	if (!needSearchVariants || rc) {
		if (isFromEdit == 1) /*Метапарсер?*/
		{
			snprintf(cmd, sizeof(cmd), "@metaparser \"%s\"", newCmd.data());
			//m_InputEdit.SetWindowTextW(utf8_to_utf16(cmd)); //(Для отладки,чтобы убедиться,что комманда приобразуется правильно)
			p = instead_cmd(cmd, &rc);
			if (rc) free(p);
		}
		if (!isFromEdit || isFromEdit > 1 || rc) {
			/* try act */
			snprintf(cmd, sizeof(cmd), "%s", newCmd.data());
			p = instead_cmd(cmd, &rc);
			//m_InputEdit.SetWindowTextW(utf8_to_utf16(cmd)); //(Для отладки,чтобы убедиться,что комманда приобразуется правильно)
			if ((textIn.Find(L"save ") == 0 || textIn.Find(L"load ") == 0) && !isFromEdit)
			{
				first_er = getError(textIn);
				free(p);
				return rc;
			}
		}
	}
	onNewInsteadCommand(cmd, p, cmdForLog, rc);
	return rc;
}

////////////////////////

BOOL CPlainInsteadView::PreTranslateMessage(MSG* pMsg)
{
	bool was_enter = false;
	// TODO: добавьте специализированный код или вызов базового класса
	if ((pMsg->message == WM_CHAR) && (GetFocus() == &m_OutEdit))
	{
		//Автоматически перескакиваем на поле ввода если начинаем набирать текст
		m_InputEdit.SetFocus();
		CString str = _T("");
		m_InputEdit.GetWindowText(str);
		str.AppendChar(pMsg->wParam);
		m_InputEdit.SetWindowText(str);
		int len = str.GetLength();
		m_InputEdit.SetSel(len, len);
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		GetFocus() == &mListScene)
	{
		int sel_pos = mListScene.GetCurSel();
		if (pos_id_scene.count(sel_pos)) {
			if (pos_id_scene[sel_pos] == 0) {
				//Это статус, по нему нельзя жмакать, просто сообщаем гудком
				MessageBeep(MB_OK);
			}
			else {
				CString res;
				int res_pos = pos_id_scene[sel_pos];
				if (res_pos < 0) res_pos = -res_pos;
				res.Format(L"%d", res_pos);
				if (inv_save_index >= 0) {
					res.Format(L"%d,%s", pos_id_inv[inv_save_index], res);
				}
				int total_list_sz = mListScene.GetCount();
				CString selText;
				mListScene.GetText(sel_pos, selText);
				TryInsteadCommand(res, L"выбор сцена \'" + selText + L"\' " + res);
				/*if (mListScene.GetCount() == total_list_sz) {
					mListScene.SetCurSel(sel_pos);
				}
				if (mListScene.GetCurSel() == LB_ERR && mListScene.GetCount() > 0)
				{
					mListScene.SetCurSel(0);
				}*/
			}
		}
		/*else if (act_on_scene.count(sel_pos)) { //Обработка прямого кода на сцене
			CString code;
code.Format(L"%s",act_on_scene[sel_pos]);
			int total_list_sz = mListScene.GetCount();
			if (inv_save_index>=0) inv_save_index =-1;
			CString selText;
			mListScene.GetText(sel_pos, selText);
			TryInsteadCommand(code, L"действие \'" + savedSelInv + L"\' на \'" + selText + L"\'");
			if (mListScene.GetCount() == total_list_sz) {
				mListScene.SetCurSel(sel_pos);
			}
			if (mListScene.GetCurSel() == LB_ERR && mListScene.GetCount() > 0)
			{
				mListScene.SetCurSel(0);
			}
		}*/
	}
	else if (pMsg->message == WM_KEYDOWN && ::GetKeyState(VK_CONTROL) < 0 && (GetFocus() == &m_OutEdit || GetFocus() == &m_InputEdit))
	{
		if (pMsg->wParam == VK_RETURN && GetFocus() == &m_InputEdit) {
			/*struct instead_args args[8];
				args[0].val = "text";
				args[0].type = INSTEAD_STR;
				args[1].val = "e";
				args[1].type = INSTEAD_STR;
				args[2].val = NULL;
				instead_lock();
				if (!instead_function("iface:input", args)) {
					MessageBoxA(0, "Тест", "", 0);
					char* cmd = instead_retval(0);
						if (cmd) {
							MessageBoxA(0, cmd, "", 0);
								free(cmd);
						}
				}
				instead_clear();
					instead_unlock();*/
			if (GlobalManager::getInstance().isUserStartGame())
			{
				was_enter = true;
				//Добавляем новую команду
				CString textIn;
				m_InputEdit.GetWindowTextW(textIn);
				GlobalManager::getInstance().appendCommand(textIn);
				m_InputEdit.SetWindowTextW(L"");
				//m_OutEdit.SetFocus();
				TryInsteadCommand(textIn, L"Конкретный ввод текста " + textIn, false, 2);
				return true;
			}
		}
		CEdit* currEdit = (CEdit*)GetFocus();
		switch (pMsg->wParam)
		{
		case 'Z':
			currEdit->Undo();
			return TRUE;
		case 'X':
			currEdit->Cut();
			return TRUE;
		case 'C':
			currEdit->Copy();
			return TRUE;
		case 'V':
			currEdit->Paste();
			return TRUE;
		case 'A':
			currEdit->SetSel(0, -1);
			return TRUE;
		}
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_UP &&
		GetFocus() == &m_InputEdit)
	{
		if (!was_enter) GlobalManager::getInstance().previosCommandMove();//) MessageBeep(MB_ICONSTOP);
		else was_enter = false;
		m_InputEdit.SetWindowText(GlobalManager::getInstance().commandData());
		m_InputEdit.SetSel(0, 0);
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_DOWN &&
		GetFocus() == &m_InputEdit)
	{
		if (!was_enter) GlobalManager::getInstance().nextCommandMove();//) MessageBeep(MB_ICONSTOP);
		else was_enter = false;
		m_InputEdit.SetWindowText(GlobalManager::getInstance().commandData());
		m_InputEdit.SetSel(0, 0);
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		GetFocus() == &m_InputEdit)
	{
		if (GlobalManager::getInstance().isUserStartGame())
		{
			was_enter = true;
			//Добавляем новую команду
			CString textIn;
			m_InputEdit.GetWindowTextW(textIn);
			GlobalManager::getInstance().appendCommand(textIn);
			//Проверяем на специальные команды
			/*CString textCheck = textIn;
			textCheck.MakeLower();
			textCheck.Trim();
			if (textCheck == L"выход")
			{
				//AfxMessageBox(L"Набрали выход!!!");
				AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_APP_EXIT, 0L);
				m_InputEdit.SetWindowTextW(L"");
				return TRUE;
			}
			else if ((textCheck == L"перезапуск") || (textCheck == L"заново"))
			{
				AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_RESTART_MENU, 0L);
				m_InputEdit.SetWindowTextW(L"");
				//GlobalManager::getInstance().userNewCommand();
				return TRUE;
			}
			else if (textCheck == L"сохранить")
			{
				AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_FILE_SAVE_GAME, 0L);
				m_InputEdit.SetWindowTextW(L"");
				GlobalManager::getInstance().userNewCommand();
				return TRUE;
			}
			else if (textCheck == L"загрузить")
			{
				AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_FILE_OPEN, 0L);
				m_InputEdit.SetWindowTextW(L"");
				GlobalManager::getInstance().userNewCommand();
								return TRUE;
								}
			*/
			//m_OutEdit.SetWindowTextW(L"");
			m_InputEdit.SetWindowTextW(L"");
			//m_OutEdit.SetFocus();
			TryInsteadCommand(textIn, L"Ввод текста " + textIn, false, true);
			return true;
		}
		else
		{
			AfxMessageBox(L"Никакая игра не выбрана. \nПожалуйста, начните новую игру");
		}
		return TRUE; // this doesn't need processing anymore
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		GetFocus() == &mListWays)
	{
		int sel_pos = mListWays.GetCurSel();
		if (pos_id_ways.count(sel_pos)) {
			if (pos_id_ways[sel_pos] == 0) {
				//Это статус, по нему нельзя жмакать, просто сообщаем гудком
				MessageBeep(MB_OK);
			}
			else {
				CString res;
				int res_pos = pos_id_ways[sel_pos];
				if (res_pos < 0) res_pos = -res_pos;
				res.Format(L"%d", res_pos);
				if (inv_save_index >= 0) {
					res.Format(L"%d,%s", pos_id_inv[inv_save_index], res);
				}
				int total_list_sz = mListWays.GetCount();
				CString selText;
				mListWays.GetText(sel_pos, selText);
				TryInsteadCommand(res, L"выбрать путь \'" + selText + L"\' " + res);
				/*if (mListWays.GetCount() == total_list_sz) {
					mListWays.SetCurSel(sel_pos);
				}
				if (mListWays.GetCurSel() == LB_ERR && mListWays.GetCount() > 0)
				{
					mListWays.SetCurSel(0);
				}*/
			}
		}
	}
	else if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		GetFocus() == &mListInv)
	{
		int sel_pos = mListInv.GetCurSel();
		if (pos_id_inv.count(sel_pos)) {
			if (pos_id_inv[sel_pos] == 0) {
				//Это статус, по нему нельзя жмакать, просто сообщаем гудком
				MessageBeep(MB_OK);
			}
			else {
				CString res;
				bool isMenuItem = (pos_id_inv[sel_pos] < 0);
				if (isMenuItem) res.Format(L"%d", -pos_id_inv[sel_pos]);
				else res.Format(L"%d", pos_id_inv[sel_pos]);
				if (inv_save_index < 0 && !isMenuItem) {
					inv_save_index = sel_pos;
					CString currText;
					mListInv.GetText(sel_pos, currText);
					savedSelInvM = currText;//сохраняем выбранный текст
					currText += L" (выбран)";
					mListInv.SetDlgItemTextW(sel_pos, currText);
					mListInv.DeleteString(sel_pos);
					mListInv.InsertString(sel_pos, currText);
					mListInv.SetCurSel(sel_pos);
				}
				else {
					if (inv_save_index >= 0 && _wtoi(res) != pos_id_inv[inv_save_index]) res.Format(L"%d,%s", pos_id_inv[inv_save_index], res);
					int total_list_sz = mListInv.GetCount();
					CString selText;
					mListInv.GetText(sel_pos, selText);
					TryInsteadCommand(res, L"действие \'" + selText + L"\' на '" + savedSelInvM + L"\' " + res);
					/*if (mListInv.GetCount() == total_list_sz) {
						mListInv.SetCurSel(sel_pos);
					}
					else if (isMenuItem && (mListInv.GetCount() > sel_pos)) {
						mListInv.SetCurSel(sel_pos);
					}
					if (mListInv.GetCurSel() == LB_ERR && mListInv.GetCount() > 0)
					{
						mListInv.SetCurSel(0);
					}*/
				}
			}
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

void CPlainInsteadView::SetOutputText(CString newText, BOOL useHistory)
{
	//Пытаемся автоматический найти меню из вывода
	if (GlobalManager::getInstance().isAutoMenuDetect() && !GlobalManager::getInstance().isUseMenu())
	{
		//CArray<CString,CString> str;
		CString field;
		int index = 0;
		while (AfxExtractSubString(field, newText, index, _T(',')))
		{
			//Нашли ключевую фразу меню
			if (field.Compare(GlobalManager::getInstance().keyMenuString()) == 0)
			{
				GlobalManager::getInstance().setUseMenu();
				break;
			}
			//str.Add(field);
			++index;
		}
	}
	if (GlobalManager::getInstance().isUseMenu())
	{
		CArray<CString, CString> str;
		CString field;
		int index = 0;
		bool findKeyVal = false;
		while (AfxExtractSubString(field, newText, index, _T(',')))
		{
			//Нашли ключевую фразу меню, после этого будем добавлять номера
			if (field.Compare(GlobalManager::getInstance().keyMenuString()) == 0)
			{

			}
			else
			{
				if (findKeyVal == false) str.Add(field);
				else
				{

					//m_ListInput.AddString(field);
				}
			}
			++index;
		}
	}
	if (GlobalManager::getInstance().isUserStartGame() && useHistory)
	{
		//Добавляем предыдущий ответ к истории
		GlobalManager::getInstance().appendLastRespond(newText);
	}
	text[0] = newText;
	text[1] = newText;
	m_OutEdit.SetWindowTextW(text[debug]);
	m_InputEdit.SetWindowTextW(L"");
	m_InputEdit.SetFocus();
	//300 мс задержка для выдачи речи
	SetTimer(ID_TIMER_1, 500, NULL);
}
void CPlainInsteadView::SendCommand(CString cmd)
{
	m_InputEdit.SetWindowTextW(cmd);
	m_InputEdit.SetFocus();
	MSG msg;
	msg.message = WM_KEYDOWN;
	msg.wParam = VK_RETURN;
	PreTranslateMessage(&msg);

}

void CPlainInsteadView::InitFocusLogic()
{
	mListScene.SetFocus();
	UpdateFocusLogic();
}

void CPlainInsteadView::UpdateFocusLogic()
{
	if (!mListScene.IsWindowEnabled() && mListScene.GetCount() > 0) mListScene.EnableWindow(true);
	if (!mListInv.IsWindowEnabled() && mListInv.GetCount() > 0) mListInv.EnableWindow(true);
	if (!mListWays.IsWindowEnabled() && mListWays.GetCount() > 0) mListWays.EnableWindow(true);

	if ((GetFocus() == &mListScene) && (mListScene.GetCount() == 0))
	{
		if (mListInv.GetCount() > 0) mListInv.SetFocus();
		else if (mListWays.GetCount() > 0) mListWays.SetFocus();
		else m_OutEdit.SetFocus();
	}
	else if ((GetFocus() == &mListInv) && (mListInv.GetCount() == 0))
	{
		if (mListScene.GetCount() > 0) mListScene.SetFocus();
		else if (mListWays.GetCount() > 0) mListWays.SetFocus();
		else m_OutEdit.SetFocus();
	}
	else if ((GetFocus() == &mListWays) && (mListWays.GetCount() == 0))
	{
		if (mListScene.GetCount() > 0) mListScene.SetFocus();
		else if (mListInv.GetCount() > 0) mListInv.SetFocus();
		else m_OutEdit.SetFocus();
	}

	if (mListScene.GetCount() == 0) mListScene.EnableWindow(false);
	if (mListInv.GetCount() == 0) mListInv.EnableWindow(false);
	if (mListWays.GetCount() == 0) mListWays.EnableWindow(false);
}

void CPlainInsteadView::OnEnSetfocusEditOut()
{
	//Убирает выделение со всего Edit, при переключении
	//Текущая позиция курсора
	int selFrom = LOWORD(m_OutEdit.GetSel());
	int selTo = HIWORD(m_OutEdit.GetSel());
	CString txtOut;
	m_OutEdit.GetWindowTextW(txtOut);
	if (selFrom == 0 && (txtOut.GetLength() == (selTo - selFrom)))
	{
		m_OutEdit.SetSel(0);
	}
}

void CPlainInsteadView::OnTimer(UINT uTime)
{
	speak(txt);
	KillTimer(ID_TIMER_1);
}

void CPlainInsteadView::UpdateSettings()
{

	CIniFile mainSettings;
	m_auto_say = mainSettings.GetInt(L"main", L"m_CheckAutosay", 1);
	m_jump_to_out = mainSettings.GetInt(L"main", L"m_CheckSetFocusToOut", 0);
	debug = mainSettings.GetInt(L"main", L"debug", 0);
	//m_BeepList = mainSettings.GetInt(L"main", L"mCheckSoundList", 1);

	UpdateFontSize();
	//Обновляем музыкальные стили
	int currWaveStyle = mainSettings.GetInt(L"main", L"m_ComboStyleAnnounce", 0);
	char wave_pos[30];
	sprintf(wave_pos, "sounds\\scene%d.wav", currWaveStyle + 1);
	if (wave_scene) {
		delete wave_scene;
		exit(0);
	}
	wave_scene = new Wave(wave_pos);

	sprintf(wave_pos, "sounds\\inventory%d.wav", currWaveStyle + 1);
	if (wave_inv) delete wave_inv;
	wave_inv = new Wave(wave_pos);

	sprintf(wave_pos, "sounds\\ways%d.wav", currWaveStyle + 1);
	if (wave_ways) delete wave_ways;
	wave_ways = new Wave(wave_pos);
}

void CPlainInsteadView::UpdateFontSize()
{
	CIniFile mainSettings;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	int currFontHeight = mainSettings.GetInt(L"main", L"fontHeight", 20);
	currInpHeight = currFontHeight + 10;
	lf.lfHeight = currFontHeight;                // Request a fontH font
	wcscpy(lf.lfFaceName, L"Arial");    // with face name "Arial".
	m_fontOut.DeleteObject();
	m_fontOut.CreateFontIndirect(&lf);    // Create the font.
	m_OutEdit.SetFont(&m_fontOut);

	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = currFontHeight;                // Request a fontH font
	wcscpy(lf.lfFaceName, L"Arial");    // with face name "Arial".
	m_fontIn.DeleteObject();
	m_fontIn.CreateFontIndirect(&lf);    // Create the font.
	m_InputEdit.SetFont(&m_fontIn);

	outFontCol = mainSettings.GetInt(L"main", L"OutFontCol", RGB(0, 0, 0));
	outBackCol = mainSettings.GetInt(L"main", L"OutBackCol", RGB(240, 240, 240));
	inFontCol = mainSettings.GetInt(L"main", L"InFontCol", RGB(0, 0, 0));
	inBackCol = mainSettings.GetInt(L"main", L"InBackCol", RGB(255, 255, 255));
}

HBRUSH CPlainInsteadView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Измените любые атрибуты DC
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_OUT)
	{
		pDC->SetTextColor(outFontCol);
		pDC->SetBkColor(outBackCol);
	}

	// TODO:  Вернуть другое значение дескриптора кисти, если оно не определено по умолчанию
	return hbr;
}

void CPlainInsteadView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: добавьте свой код обработчика сообщений
}

void CPlainInsteadView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);

	// TODO: добавьте свой код обработчика сообщений
}

void CPlainInsteadView::OnClose()
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного

	CFormView::OnClose();
}


static int countExacWnd = 0;
static CString lastWndText = L"";
static CString currWindText = L"";
#define MAX_STABLE_COUNT 3

static void AppendTextToEditCtrl(CEdit& edit, LPCTSTR pszText)
{
	// get the initial text length
	int nLength = edit.GetWindowTextLength();
	// put the selection at the end of text
	edit.SetSel(nLength, nLength);
	// replace the selection
	edit.ReplaceSel(pszText);
}

void CALLBACK EXPORT OnTimerUpdateText(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	//Только если начали игру
	if (GlobalManager::getInstance().isUserStartGame())
	{
		/*
		if (!WasReadScreen)
		{
			CString toDisp(temp_buff);

			CPlainInsteadView::GetCurrentView()->SetOutputText(toDisp);

			//WasReadScreen = TRUE;
		}
		*/
		//Открываем диалоговые окна
		/*
		if (AskSaveLoadTadsGame)
		{
			KillTimer(hWnd, ID_TIMER_2); //Останавливаем таймер

			CFileDialog* dlg;
			CFileDialog fileSaveDialog(FALSE,L"*.sav",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Tads save (*.sav)|*.sav");	//объект класса выбора файла
			CFileDialog fileOpenDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Tads save (*.sav)|*.sav");	//объект класса выбора файла
			if (IsNeedSaveTadsGame) dlg = &fileSaveDialog;
			else dlg = &fileOpenDialog;
			CString dir;
			GetCurrentDirectory( MAX_PATH, CStrBuf(dir, MAX_PATH) );
			dir.Append(L"\\saves");
			dlg->m_ofn.lpstrInitialDir  = dir;
			int result = dlg->DoModal();	//запустить диалоговое окно
			if (result==IDOK)	//если файл выбран
			{
				//Очищаем поле вывода
				if (IsNeedSaveTadsGame) GlobalManager::getInstance().userSavedFile();
				CPlainInsteadView::GetCurrentView()->SetOutputText(L"");
				GlobalManager::lastString = 0;
				CString filePath = dlg->GetPathName();
				CT2A ascii(filePath);
				strcpy(TadsFileName,ascii);
				IsOkSaveLoadTadsGame = TRUE;
			}
			//Завершаем запрос
			AskSaveLoadTadsGame = FALSE;
			//Снова запускаем таймер обновления текста
			SetTimer(hWnd, ID_TIMER_2,100,OnTimerUpdateText);
		}
		*/
	}
}

void CPlainInsteadView::OnFullHistory()
{
	//Просмотр всей истории, если она есть
	if (GlobalManager::getInstance().isUserStartGame())
	{
		CString fullHist = GlobalManager::getInstance().fullHistoryData();
		if (!fullHist.IsEmpty())
		{
			SetOutputText(fullHist, FALSE);
		}
	}
}

void CPlainInsteadView::OnBackHist()
{
	//Двигаемся назад по истории
	if (GlobalManager::getInstance().isUserStartGame())
	{
		CString prevStep = GlobalManager::getInstance().previosHistoryData();
		if (!prevStep.IsEmpty())
		{
			SetOutputText(prevStep, FALSE);
		}
	}
}

void CPlainInsteadView::OnForwHist()
{
	//Двигаемся вперёд по истории
	if (GlobalManager::getInstance().isUserStartGame())
	{
		CString nextStep = GlobalManager::getInstance().nextHistoryData();
		if (!nextStep.IsEmpty())
		{
			SetOutputText(nextStep, FALSE);
		}
	}
}

void CPlainInsteadView::ShowTextFromResource(LPCWSTR res_id)
{
	bool ok = false;
	HRSRC hResource = FindResource(NULL, res_id, L"Text");

	if (hResource)
	{
		HGLOBAL hLoadedResource = LoadResource(NULL, hResource);
		if (hLoadedResource)
		{
			LPVOID pLockedResource = LockResource(hLoadedResource);
			if (pLockedResource)
			{
				DWORD dwResourceSize = SizeofResource(NULL, hResource);
				if (0 != dwResourceSize)
				{
					// Use pLockedResource and dwResourceSize however you want
					LPBYTE sData = (LPBYTE)pLockedResource;
					char* text = (char*)sData;
					CString sText(text);
					SetOutputText(sText, FALSE);
					ok = true;
				}
			}
		}
	}

	if (ok == false)
	{
		SetOutputText(L"Ошибка. Не могу отобразить текст справки.", FALSE);
	}
}

void CPlainInsteadView::OnManualStarter()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT1));
}

void CPlainInsteadView::OnManualCmdList()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT2));
}

void CPlainInsteadView::OnManualHowPlay()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT3));
}
void CPlainInsteadView::OnManualRuk1()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT4));
}

void CPlainInsteadView::OnManualRuk2()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT5));
}

void CPlainInsteadView::OnManualRuk3()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT6));
}

void CPlainInsteadView::OnManualRuk4()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT7));
}

void CPlainInsteadView::OnManualRuk5()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT8));
}

void CPlainInsteadView::OnManualRuk6()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT9));
}

void CPlainInsteadView::OnManualRuk7()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT10));
}

void CPlainInsteadView::OnManualBigWrap()
{
	ShowTextFromResource(MAKEINTRESOURCE(IDR_TEXT11));
}

void CPlainInsteadView::OnFindText()
{
	if (NULL == m_pFindDialog)
	{
		m_pFindDialog = new CFindReplaceDialog(); // Must be created on the heap.
		m_pFindDialog->Create(TRUE, _T(""), _T(""), FR_DOWN | FR_HIDEUPDOWN | FR_HIDEWHOLEWORD, this);
		m_pFindDialog->m_fr.lStructSize = sizeof(FINDREPLACE);
		m_pFindDialog->m_fr.hwndOwner = this->m_hWnd;
	}
}

LRESULT CPlainInsteadView::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pFindDialog != NULL);

	// If the FR_DIALOGTERM flag is set,
	// Закрываем окно
	if (m_pFindDialog->IsTerminating())
	{
		m_pFindDialog = NULL;
		return 0;
	}

	// If the FR_FINDNEXT flag is set,
	// Ищем строку
	if (m_pFindDialog->FindNext())
	{
		//read data from dialog
		CString FindName = m_pFindDialog->GetFindString();
		bool bMatchCase = m_pFindDialog->MatchCase() == TRUE;
		//bool bMatchWholeWord = m_pFindDialog->MatchWholeWord() == TRUE;
		//bool bSearchDown = m_pFindDialog->SearchDown() == TRUE;

		//with given name do search
		if (FindStringInEdit(FindName, bMatchCase))
		{
			wasFind = true;
			lastSearchStr = FindName;
			lastMatchCase = bMatchCase;

			m_pFindDialog->DestroyWindow();
			m_pFindDialog = NULL;
			return 0;
		}
		else
		{
			MessageBeep(MB_ICONSTOP);
		}
	}

	return 0;
}

bool CPlainInsteadView::FindStringInEdit(CString FindName, bool bMatchCase)
{
	//Поиск в editBox
	CString sEdit;
	m_OutEdit.GetWindowTextW(sEdit);
	//Если не надо учитывать регистр, то выравнием буквы
	if (!bMatchCase)
	{
		FindName.MakeLower();
		sEdit.MakeLower();
	}
	//Текущая позиция курсора
	int sel = LOWORD(m_OutEdit.GetSel());
	//Ищем вниз
	int resPos = sEdit.Find(FindName, sel + 1);
	//Нашли - выделяем
	if (resPos >= 0)
	{
		m_OutEdit.SetSel(resPos, resPos + FindName.GetLength());
		return true;
	}
	return false;
}

void CPlainInsteadView::OnFindNext()
{
	if (wasFind)
	{
		if (!FindStringInEdit(lastSearchStr, lastMatchCase)) MessageBeep(MB_ICONSTOP);
	}
}

void CPlainInsteadView::OnHistoryStop()
{
	//Остановка записи истории и команд
	GlobalManager::getInstance().enableHistory(false);
	SetOutputText(L"ИСТОРИЯ ОСТАНОВЛЕНА", FALSE);
}

void CPlainInsteadView::OnHistoryStart()
{
	//Возобновление записи истории
	GlobalManager::getInstance().enableHistory(true);
	SetOutputText(L"ИСТОРИЯ ПРОДОЛЖЕНА", FALSE);
}


void CPlainInsteadView::OnUpdateOutView()
{
	// TODO: добавьте свой код обработчика команд
	if (GlobalManager::getInstance().isUserStartGame())
	{
		/*int sel_pos = 0;
		CListBox* curr_box = 0;
		if (GetFocus() == &mListInv) {
			sel_pos = mListInv.GetCurSel();
			curr_box = &mListInv;
		}
		else if (GetFocus() == &mListScene) {
			sel_pos = mListScene.GetCurSel();
			curr_box = &mListScene;
		}
		else if (GetFocus() == &mListWays) {
			sel_pos = mListWays.GetCurSel();
			curr_box = &mListWays;
		}*/
		speak(txt);
		/*if (!m_jump_to_out && curr_box)
		{
			if (curr_box->GetCount() > sel_pos) {
				curr_box->SetCurSel(sel_pos);
			}
		}*/
	}
}


void CPlainInsteadView::OnStnClickedStaticScene()
{
	// TODO: добавьте свой код обработчика уведомлений
}


void CPlainInsteadView::OnLbnSetfocusListScene()
{
	if (GetFocus() == &mListScene)
	{
		/*mListInv.SetCurSel(-1);
		mListWays.SetCurSel(-1);*/
		MultiSpeech::getInstance().Say(L"объекты");
	}
	/*if (mListScene.GetCurSel() == LB_ERR && mListScene.GetCount() > 0)
	{
		mListScene.SetCurSel(0);
	}*/
}


void CPlainInsteadView::OnLbnSetfocusListInv()
{
	if (GetFocus() == &mListInv) {
		/*mListWays.SetCurSel(-1);
		mListScene.SetCurSel(-1);*/
		MultiSpeech::getInstance().Say(L"инвентарь");
	}
	// TODO: добавьте свой код обработчика уведомлений
	/*if (mListInv.GetCurSel() == LB_ERR && mListInv.GetCount() > 0)
	{
		mListInv.SetCurSel(0);
	}*/
}


void CPlainInsteadView::OnLbnSetfocusListWays()
{
	if (GetFocus() == &mListWays)
	{
		//mListScene.SetCurSel(-1);
		MultiSpeech::getInstance().Say(L"пути");
	}
	// TODO: добавьте свой код обработчика уведомлений
	/*if (mListWays.GetCurSel() == LB_ERR && mListWays.GetCount() > 0)
	{
		mListWays.SetCurSel(0);
	}*/
}


void CPlainInsteadView::OnGotoScene()
{
	if (mListScene.GetCount() > 0 && GetFocus() != &mListScene)
	{
		mListScene.SetFocus();
	}
}


void CPlainInsteadView::OnGotoInv()
{
	if (mListInv.GetCount() > 0 && GetFocus() != &mListInv)
	{
		mListInv.SetFocus();
	}
}


void CPlainInsteadView::OnGotoWays()
{
	if (mListWays.GetCount() > 0 && GetFocus() != &mListWays)
	{
		mListWays.SetFocus();
	}
}

void CPlainInsteadView::OnMenuLog()
{
	// TODO: добавьте свой код обработчика команд
	isLogOn = !isLogOn;
	CMenu* pMenu = AfxGetApp()->GetMainWnd()->GetMenu();
	if (pMenu != NULL)
	{
		if (isLogOn) {
			pMenu->CheckMenuItem(ID_MENU_LOG, MF_CHECKED | MF_BYCOMMAND);
			// uses printf() format specifications for time
			CString t = CTime::GetCurrentTime().Format("%y%m%d_%H%M");
			logFileName = getLogsDir() + L"log_" + t + L".txt";
			AfxMessageBox(L"Логирование включено. Лог сохраниться в папке logs под именем: " + logFileName);
		}
		else
		{
			pMenu->CheckMenuItem(ID_MENU_LOG, MF_UNCHECKED | MF_BYCOMMAND);
		}
	}
}
void CPlainInsteadView::OnMenuDebug() {
	debug = !debug;
	CMenu* pMenu = AfxGetApp()->GetMainWnd()->GetMenu();
	if (pMenu != NULL) {
		CIniFile mainSettings;
		mainSettings.WriteNumber(L"main", L"debug", debug);
		pMenu->CheckMenuItem(ID_MENU_DEBUG, (debug ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);
		updateText();
	}
}
void CPlainInsteadView::TurnOffLogging()
{
	if (isLogOn) OnMenuLog();
}
