#include "StdAfx.h"
#include "MultiSpeech.h"
#include "Tolk.h"

MultiSpeech::MultiSpeech()
{
	 Tolk_Load();
}

void MultiSpeech::Say(CString text)
{
	//Говорим всем чем только возможно
	if (Tolk_HasSpeech()) 
	{
		 if (!Tolk_Output(text)) {
			TRACE( L"Failed to output text" );
		}
	}
}

CString MultiSpeech::GetCurrentReader()
{
	CString res(Tolk_DetectScreenReader());
	return res;
}

void MultiSpeech::Unload()
{
	Tolk_Unload();
}

MultiSpeech::~MultiSpeech()
{
}
