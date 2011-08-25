/**
 * @author Alexander Tenberge
 * @date Jun 5, 2011
 * @file sound/soundmgr.hpp
 */


#ifndef SOUNDMGR_HPP_
#define SOUNDMGR_HPP_

#include <fmodex/fmod.hpp>
#include <fmodex/fmod_errors.h>
#include <string>
#include <map>
#include <queue>
#include <boost/thread.hpp>
#include <iostream>

namespace snd {

struct SoundEvent {
	std::string name;
	float volume;
	FMOD_VECTOR position;
	FMOD_VECTOR velocity;
};

class SoundMgr {
private:
	FMOD::System* m_system;
	FMOD::Channel* m_musicChannel;
	bool m_musicEnabled;
	std::map<std::string, FMOD::Sound*>::iterator m_currentMusic;
	std::map<std::string, FMOD::Sound*> m_sounds;
	std::map<std::string, FMOD::Sound*> m_music;
	std::queue<SoundEvent*> m_soundQueue;
	boost::mutex m_mutex;

	static SoundMgr* s_instance;
	SoundMgr();
	SoundMgr(const SoundMgr& other);
	virtual ~SoundMgr();
	inline static bool ERRCHECK(FMOD_RESULT result);
protected:
public:
	static SoundMgr& instance();
	static void destroy();

	void setMusicEnabled(bool enabled);

	void SoundUpdate();
	void SetListenerPos(float* listenerpos, float* forward, float* upward, float* velocity);
	unsigned LoadSound(const std::string& folder);
	unsigned LoadMusic(const std::string& folder);
	void PlaySound(const std::string& name, float volume, float* position, float* velocity);
	void PlayMusic(float volume);
};

inline
void SoundMgr::setMusicEnabled(bool enabled)
{
	if (!enabled && m_musicChannel) {
		m_musicChannel->stop();
		m_musicChannel = NULL;
	}
	m_musicEnabled = enabled;
}

inline
bool SoundMgr::ERRCHECK(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		std::cerr << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
		return false;
	}
	return true;
}

}

#endif /* SOUNDMGR_HPP_ */
