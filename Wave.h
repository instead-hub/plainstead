#pragma once
#include <stdafx.h>

#include "bass.h"

class Wave {
public:
	Wave(char *filename);
	~Wave();
	void play();
	bool isok();
private:
	HSAMPLE sample;
	//�� ���� MMSYSTEM
	//char *buffer;
	//bool ok;
	//HINSTANCE HInstance;
};