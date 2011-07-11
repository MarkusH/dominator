/*
 * SoundMgr.hpp
 *
 *  Created on: 09.06.2011
 *      Author: Alexander Tenberge
 */

#ifndef SOUNDMGR_HPP_
#define SOUNDMGR_HPP_

#include <fmodex/fmod.hpp>
#include <fmodex/fmod_errors.h>
#include <string>
#include <map>
#include <list>

namespace snd {

class SoundMgr {
private:
	FMOD::System     *m_system;
	FMOD::Channel    *m_musicChannel;
	std::map<std::string, FMOD::Sound*>::iterator m_currentMusic;
	std::map<std::string, FMOD::Sound*> m_sounds;
	std::map<std::string, FMOD::Sound*> m_music;

	static SoundMgr* s_instance;
	SoundMgr();
	SoundMgr(const SoundMgr& other);
	virtual ~SoundMgr();
	void ERRCHECK(FMOD_RESULT result);
	void LoadFileIntoMemory(const char *name, void **buff, int *length);
protected:
public:
	static SoundMgr& instance();
	static void destroy();

	void SoundUpdate();
	void SetListenerPos(float* listenerpos, float* forward, float* upward, float* velocity);
	unsigned LoadSound(const std::string& folder);
	unsigned LoadMusic(const std::string& folder);
	void PlaySound(const char *name, int volume, float* position, float* velocity);
	void PlayMusic(int volume);
};

}

#endif /* SOUNDMGR_HPP_ */
