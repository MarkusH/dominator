/*
 * SoundMgr.cpp
 *
 *  Created on: 09.06.2011
 *      Author: Alexander Tenberge
 */

#include <sound/SoundMgr.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <boost/filesystem.hpp>

namespace snd {

SoundMgr* SoundMgr::s_instance = NULL;

SoundMgr::SoundMgr()
{
	m_currentMusic = m_music.begin();
	m_musicChannel = NULL;
	 FMOD_RESULT result;

	 unsigned int version;

	 result = FMOD::System_Create(&m_system);
	 ERRCHECK(result);

	 result = m_system->getVersion(&version);
	 ERRCHECK(result);

	 if (version < FMOD_VERSION)
	 {
	     std::cout << "Error! You are using an old version of FMOD " << version << ". " <<  "This program requires " << FMOD_VERSION << "." << std::endl;
	 }

	 result = m_system->init(32, FMOD_INIT_NORMAL, 0);
	 ERRCHECK(result);
	 result = m_system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
	 ERRCHECK(result);
}

SoundMgr::SoundMgr(const SoundMgr& other)
{
}

SoundMgr::~SoundMgr()
{
	FMOD_RESULT       result;
	result = m_system->close();
	ERRCHECK(result);
	result = m_system->release();
	ERRCHECK(result);
}

void SoundMgr::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        std::cout << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
    }
}

void SoundMgr::LoadFileIntoMemory(const char *name, void **buff, int *length)
{
    FILE *fp = fopen(name, "rb");

    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *buff = malloc(*length);
    fread(*buff, *length, 1, fp);

    fclose(fp);
}

SoundMgr& SoundMgr::instance()
{
	if (!s_instance)
		s_instance = new SoundMgr();
	return *s_instance;
}

void SoundMgr::destroy()
{
	if (s_instance)
		delete s_instance;
}

void SoundMgr::SoundUpdate()
{
	m_system->update();
	bool playing;
	m_musicChannel->isPlaying(&playing);
	if (!playing) {
		PlayMusic(100);
	}
}

void SoundMgr::SetListenerPos(float* listenerpos, float* forward, float* upward, float* velocity)
{
	FMOD_RESULT result;
	FMOD_VECTOR pos;
	pos.x=listenerpos[0] * 0.025f;
	pos.y=listenerpos[1] * 0.025f;
	pos.z=listenerpos[2] * 0.025f;
	FMOD_VECTOR dir;
	dir.x=-forward[0];
	dir.y=-forward[1];
	dir.z=-forward[2];
	FMOD_VECTOR up;
	up.x=upward[0];
	up.y=upward[1];
	up.z=upward[2];
	FMOD_VECTOR vel;
	vel.x=velocity[0];
	vel.y=velocity[1];
	vel.z=velocity[2];

    result = m_system->set3DListenerAttributes(0, &pos, &vel, &dir, &up);
    ERRCHECK(result);
}

unsigned SoundMgr::LoadSound(const std::string& folder)
{
	int count = 0;
	FMOD::Sound      *sound;
	void             *buff;
	int               length;
	FMOD_RESULT       result;
	FMOD_CREATESOUNDEXINFO exinfo;

	using namespace boost::filesystem;

		path p (folder);

		if(is_directory(p)) {
			if(!is_empty(p)) {
				directory_iterator end_itr;
				for(directory_iterator itr(p); itr != end_itr; ++itr) {
					if(itr->leaf().size() > 3) {
							count++;

							result = m_system->createSound(itr->string().c_str(), FMOD_HARDWARE | FMOD_3D, 0, &sound);
							ERRCHECK(result);

							result = sound->set3DMinMaxDistance(0.5f, 200000.0f);
						    ERRCHECK(result);

							result = sound->setMode(FMOD_LOOP_OFF);
							ERRCHECK(result);

							m_sounds[basename(*itr)]=sound;
					}
				}
			}

		}
	return count;
}

unsigned SoundMgr::LoadMusic(const std::string& folder)
{
	int count = 0;
	FMOD::Sound      *sound;
	FMOD_RESULT       result;
	using namespace boost::filesystem;

		path p (folder);

		if(is_directory(p)) {
			if(!is_empty(p)) {
				directory_iterator end_itr;
				for(directory_iterator itr(p); itr != end_itr; ++itr) {
					if(itr->leaf().size() > 3) {
						count++;
						result = m_system->createSound(itr->string().c_str(), FMOD_HARDWARE | FMOD_CREATESTREAM, 0, &sound);
						ERRCHECK(result);
						result = sound->setMode(FMOD_LOOP_OFF);
						ERRCHECK(result);
						m_music[basename(*itr)]=sound;
					}
				}
			}

		}
		m_currentMusic = m_music.begin();
	return count;
}

void SoundMgr::PlaySound(const char *name, int volume, float* position, float* velocity)
{
	FMOD_RESULT result;
	std::map<std::string, FMOD::Sound*>::iterator itr= m_sounds.find(name);

	if(itr!=m_sounds.end())
	{
		FMOD::Channel* channel;
		result = m_system->playSound(FMOD_CHANNEL_FREE, itr->second, true, &channel);
		ERRCHECK(result);
		FMOD_VECTOR pos;
		pos.x=position[0] * 0.025f;
		pos.y=position[1] * 0.025f;
		pos.z=position[2] * 0.025f;
		FMOD_VECTOR vel;
		vel.x=velocity[0];
		vel.y=velocity[1];
		vel.z=velocity[2];
        result = channel->set3DAttributes(&pos, &vel);
        ERRCHECK(result);
        channel->setVolume(volume * 100.0f);
        result = channel->setPaused(false);
        ERRCHECK(result);
	}


}

void SoundMgr::PlayMusic(int volume)
{
	FMOD_RESULT result;

		if(m_currentMusic!=m_music.end())
		{
			m_currentMusic++;
			if(m_currentMusic==m_music.end())
				m_currentMusic = m_music.begin();
			result = m_system->playSound(FMOD_CHANNEL_FREE, m_currentMusic->second, false, &m_musicChannel);
			ERRCHECK(result);
		}
}


}
