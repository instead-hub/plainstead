#include "Wave.h"
#include <iostream>
#include <fstream>

int Wave::currVol = 100;

void Wave::SetVolume(int vol)
{
	if (vol < 0) currVol = 0;
	else if (vol>100) currVol = 100;
	else currVol = vol;
}

int Wave::GetVolume()
{
	return currVol;
}

Wave::Wave(char *filename)
{
	sample = BASS_SampleLoad(FALSE, filename, 0, 0, 1, 0);
}

void Wave::play()
{
	if (sample) {
		HCHANNEL chan = BASS_SampleGetChannel(sample, FALSE);
		if (chan) {
			BASS_ChannelSetAttribute(chan, BASS_ATTRIB_VOL, GetVolume()/100.0f);
			BASS_ChannelPlay(sample, FALSE);
		}
	}
}

bool Wave::isok()
{
	return (sample);
}

Wave::~Wave()
{
	if (sample) BASS_SampleFree(sample);
}
//На базе MMSYSTEM
/*
#include <mmsystem.h>

Wave::Wave(char *filename)
{
	ok = false;
	buffer = 0;
	HInstance = GetModuleHandle(0);

	std::ifstream infile(filename, std::ios::binary);

	if (!infile)
	{
		std::cout << "Wave::file error: " << filename << std::endl;
		return;
	}

	infile.seekg(0, std::ios::end);   // get length of file
	int length = infile.tellg();
	buffer = new char[length];    // allocate memory
	infile.seekg(0, std::ios::beg);   // position to start of file
	infile.read(buffer, length);  // read entire file

	infile.close();
	ok = true;
}

Wave::~Wave()
{
	PlaySound(NULL, 0, 0); // STOP ANY PLAYING SOUND
	delete[] buffer;      // before deleting buffer.
}
void Wave::play(bool async)
{
	if (!ok)
		return;

	if (async)
		PlaySound((LPCWSTR)buffer, HInstance, SND_MEMORY | SND_ASYNC | SND_NOSTOP);
	else
		PlaySound((LPCWSTR)buffer, HInstance, SND_MEMORY | SND_NOSTOP);
}

bool Wave::isok()
{
	return ok;
}
*/