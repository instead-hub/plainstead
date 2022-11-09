#pragma once
#include <stdafx.h>
#include "bass.h"
/*#include <windows.h>
#include <mmsystem.h>
#pragma comment( lib, "Winmm.lib" )*/
class Wave {
public:
	static void SetVolume(int vol);
	static int GetVolume();
	static void Play(char* sound,int isLooping);
	Wave(char *filename);
	~Wave();
	void play(bool ignore_settings = false);
	bool isok();
private:
	HSAMPLE sample;
	static int currVol;
	//На базе MMSYSTEM
	//char *buffer;
	//bool ok;
	//HINSTANCE HInstance;
};