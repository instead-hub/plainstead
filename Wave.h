#pragma once
#include <stdafx.h>

#include "bass.h"

class Wave {
public:
	static void SetVolume(int vol);
	static int GetVolume();

	Wave(char *filename);
	~Wave();
	void play();
	bool isok();
private:
	HSAMPLE sample;
	static int currVol;
	//На базе MMSYSTEM
	//char *buffer;
	//bool ok;
	//HINSTANCE HInstance;
};