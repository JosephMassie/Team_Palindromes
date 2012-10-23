#include "Sound.h"

void Sound::initialize()
{
	////////////////////////////////////////////////////////////////////////////
	// Set up FMOD and sounds
	///////////////////////////////////////////////////////////////////////////
	// create system
	FMOD::System_Create(&system);
	// get version
	system->getVersion(&version);
	// get number of drivers
	system->getNumDrivers(&numdrivers);
	// get driver caps
	system->getDriverCaps(0, &caps, 0, &speakermode);
	// get driver info
	system->getDriverInfo(0, name, 256, 0);
	// initialize system
	system->init(100, FMOD_INIT_NORMAL, 0);

	// create sounds
	system->createSound("audio/hit.wav", FMOD_DEFAULT, 0, &m_soundEffects[HIT]);
	system->createSound("audio/press.wav", FMOD_DEFAULT, 0, &m_soundEffects[PRESS]);
	system->createSound("audio/fire.wav", FMOD_DEFAULT, 0, &m_soundEffects[FIRE]);
}

Sound::Sound()
{
}

Sound* Sound::getInstance()
{
	static Sound instance;
	return &instance;
}

Sound::~Sound()
{
	release();
}

void Sound::update()
{
	system->update();
}

void Sound::playSound(int sound)
{
	system->playSound(FMOD_CHANNEL_FREE, m_soundEffects[sound], false, 0);
}

void Sound::release()
{
	for(int i = 0; i < 5; ++i)
	{
		if(m_soundEffects[i])
			m_soundEffects[i]->release();
	}
	if(system)
		system->release();
}