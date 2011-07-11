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
	 channel = 0;
	 buff = 0;
	 length = 0;
	 //listenerpos = { 0.0f, 0.0f, 0.0f };
	 listenerpos.x = 0.0f;
	 listenerpos.y = 0.0f;
	 listenerpos.z = 0.0f;

	 result = FMOD::System_Create(&system);
	 ERRCHECK(result);

	 result = system->getVersion(&version);
	 ERRCHECK(result);

	 if (version < FMOD_VERSION)
	 {
	     std::cout << "Error! You are using an old version of FMOD " << version << ". " <<  "This program requires " << FMOD_VERSION << "." << std::endl;
	     exit(EXIT_FAILURE);
	 }

	 result = system->init(32, FMOD_INIT_NORMAL, 0);
	 ERRCHECK(result);
}

SoundMgr::SoundMgr(const SoundMgr& other)
{
}

SoundMgr::~SoundMgr()
{
	SoundList.~list();
	result = system->close();
	ERRCHECK(result);
	result = system->release();
	ERRCHECK(result);
}

void SoundMgr::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        std::cout << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void SoundMgr::LoadFileIntoMemory(const char *name, void **buff, int *length)//test
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
	system->update();
}

void SoundMgr::SetListenerPos()
{


    //result = system->set3DListenerAttributes(0, &listenerpos, &vel, &forward, &up);//brauch ich die params?
    //ERRCHECK(result);
}

unsigned SoundMgr::LoadSound(std::string folder)
{
	int count = 0;//braucht man count fuer irgendwas?

	using namespace boost::filesystem;

		path p (folder);

		if(is_directory(p)) {
			if(!is_empty(p)) {
				directory_iterator end_itr;
				for(directory_iterator itr(p); itr != end_itr; ++itr) {
					if(itr->leaf().size() > 3) {
							count++;

							LoadFileIntoMemory(itr->string().c_str(), &buff, &length);
							memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
							exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
							exinfo.length = length;

							result = system->createSound((const char *)buff, FMOD_HARDWARE | FMOD_OPENMEMORY | FMOD_3D, &exinfo, &sound1);
							ERRCHECK(result);

						    result = sound1->set3DMinMaxDistance(4.0f, 10000.0f);
						    ERRCHECK(result);

							result = sound1->setMode(FMOD_LOOP_OFF);
							ERRCHECK(result);

							SoundList.push_front(*sound1);

							free(buff);
					}
				}
			}

		}
	return count;
}

void SoundMgr::PlaySound(const char *name, int volume)//position param?
{
	//sound suchen
	for(std::list<FMOD::Sound>::iterator itr=SoundList.begin(); itr!=SoundList.end(); itr++)
	{
		//@todo storing the name will be needed

	//result = system->playSound(FMOD_CHANNEL_FREE, itr, false, &channel);
	//ERRCHECK(result);
	}

}

void SoundMgr::PlayMusic(const char *name, int volume)
{

}


}
