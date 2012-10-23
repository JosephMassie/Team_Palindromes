#pragma once
//////////////////////////////////////////////////////////////////////////
// FMOD
//////////////////////////////////////////////////////////////////////////
#include <fmod.h>
#include <fmod.hpp>
#pragma comment(lib, "Fmodex_vc.lib")

#define HIT 0
#define PRESS 1
#define FIRE 2

class Sound
{
private:
	FMOD::System *system;
	FMOD_RESULT result;
	unsigned int version;
	int numdrivers;
	FMOD_SPEAKERMODE speakermode;
	FMOD_CAPS caps;
	char name[256];

	FMOD::Sound *m_soundEffects[3];
	FMOD::Channel *m_BGMChannel;
	Sound();
public:
	static Sound* getInstance();
	~Sound();
	void update();
	void initialize();
	void release();
	void playSound(int sound);
};