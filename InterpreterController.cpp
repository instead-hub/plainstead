#include "stdafx.h"
#include "InterpreterController.h"
#include <windows.h>
#include <Tlhelp32.h>
#include <atlconv.h>
#include <afxconv.h>
#include <GlobalManager.h>
#include <string>

extern "C" {
	#include "instead\instead.h"
}

#pragma setlocale("Russian_Russia.1251")

CString InterpreterController::m_gameFile=L"";
CString InterpreterController::m_gameName=L"";
CString InterpreterController::m_lastCommand=L"";
bool InterpreterController::m_wasCommand=false;

InterpreterController::InterpreterController(void)
{
}


InterpreterController::~InterpreterController(void)
{
}
/*
static void killProcessByName(CString filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (wcscmp(pEntry.szExeFile, filename) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}
*/

void InterpreterController::startGameFile(CString gameFile, CString gameName, int autolog)
{
    startGameFile(gameFile,gameName,L"",autolog);
}

HANDLE hMachine = NULL;
DWORD tidMachine = 0;

void InterpreterController::loadFile(CString gameFile,CString gameName) {
    char* str;
    if (instead_load(&str) == 0)
    {
        m_gameFile = gameFile;
        m_gameName = gameName;
        m_lastCommand = L"";
        m_wasCommand = true;
        free(str);
    }

}
void InterpreterController::startGameFile(CString gameFile, CString gameName, CString saveFile, int autolog)
{
    //Чтобы не появлялся диалог с предложением сохраниться,когда мы начинаем новую игру,или начинаем игру заново.
GlobalManager::getInstance().userSavedFile();
    //TODO: добавить старт игры
    CT2A ascii(gameFile);
    if (!saveFile.IsEmpty() && strcmp(instead_get_api(), "stead3") == 0) {
        //Ничего не делаем,т.к всё загрузится автоматом.
         //loadFile(gameFile,gameName);
        return;
}
instead_done();
    instead_set_debug(1);
    if (instead_init(ascii) == 0) loadFile(gameFile, gameName);
}

CString InterpreterController::RunInterpreter(CString command)
{
    CString Result;

    //Добавляем команду, на выдачу в интерпретатор
	CT2A ascii(command);
    //writeToCommonOut(ascii);
	m_lastCommand = command;
	m_wasCommand = true;

    return Result;
}

void InterpreterController::endInterpreter()
{
	//killProcessByName(L"t2r32.exe");
}
//Загрузка сохранения в интерпретатор
bool InterpreterController::loadSave(CString fname)
{
	startGameFile(m_gameFile,m_gameName,fname,1);
	return true;
}

bool InterpreterController::saveGame(CString fname)
{
	return CopyFile(L"temp\\last.sav",fname,FALSE);
}
CString InterpreterController::lastCommand()
{
	return m_lastCommand;
}
void InterpreterController::clearNewCommandFlag()
{
	m_wasCommand = false;
}


bool InterpreterController::wasNewCommand()
{
	return m_wasCommand;
}

static bool Utf8ToCString( CString& cstr, const char* utf8Str )
{
    size_t utf8StrLen = strlen(utf8Str);

    if( utf8StrLen == 0 )
    {
        cstr.Empty();
        return true;
    }

    LPWSTR ptr = cstr.GetBuffer(utf8StrLen+1);

    // CString is UNICODE string so we decode
    int newLen = MultiByteToWideChar(
                     CP_UTF8,  0,
                     utf8Str, utf8StrLen,  ptr, utf8StrLen+1
                     );
    if( !newLen )
    {
        cstr.ReleaseBuffer(0);
        return false;
    }

    cstr.ReleaseBuffer(newLen);
    return true;
}

std::string utf8_encode(const std::wstring &wstr)
{
    if( wstr.empty() ) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}
