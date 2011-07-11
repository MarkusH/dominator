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
#include <list>

namespace snd {

class SoundMgr {
private:
	FMOD::System     *system;
	FMOD::Sound      *sound1;
	FMOD::Channel    *channel;
	FMOD_RESULT       result;
	int               key;
	unsigned int      version;
	void             *buff;
	int               length;
	FMOD_CREATESOUNDEXINFO exinfo;
    FMOD_VECTOR       listenerpos;
    std::list<FMOD::Sound> SoundList;

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
	void SetListenerPos();
	//FMOD::Sound* LoadSound();//anderer return?
	unsigned LoadSound(std::string folder);
	void PlaySound(const char *name, int volume);//param: pos
	void PlayMusic(const char *name, int volume);//wie anhalten?
};

}

#endif /* SOUNDMGR_HPP_ */
