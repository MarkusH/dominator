/**
 * @author Alexander Tenberge
 * @date Jun 5, 2011
 * @file sound/soundmgr.cpp
 */

#include <sound/soundmgr.hpp>
#define BOOST_FILESYSTEM_VERSION 2
#include <boost/filesystem.hpp>

#define SOUND_MAX_CHANNELS 32

namespace snd {

SoundMgr* SoundMgr::s_instance = NULL;

SoundMgr& SoundMgr::instance() {
	if (!s_instance)
		s_instance = new SoundMgr();
	return *s_instance;
}

void SoundMgr::destroy() {
	if (s_instance)
		delete s_instance;
}

SoundMgr::SoundMgr()
{
	m_musicEnabled = false;
	m_currentMusic = m_music.begin();
	m_musicChannel = NULL;
	FMOD_RESULT result;

	unsigned int version;

	result = FMOD::System_Create(&m_system);
	ERRCHECK(result);

	result = m_system->getVersion(&version);
	ERRCHECK(result);

	if (version < FMOD_VERSION) {
		std::cerr << "Error! You are using an old version of FMOD " << version
				<< ". This program requires " << FMOD_VERSION << "." << std::endl;
	}

	//result = m_system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
	//ERRCHECK(result);

	result = m_system->init(SOUND_MAX_CHANNELS, FMOD_INIT_NORMAL, 0);
	ERRCHECK(result);
}

SoundMgr::SoundMgr(const SoundMgr& other)
{
}

SoundMgr::~SoundMgr()
{
	FMOD_RESULT result;
	result = m_system->close();
	ERRCHECK(result);
	result = m_system->release();
	ERRCHECK(result);
}

void SoundMgr::SoundUpdate()
{
	FMOD_RESULT result;
	int channels = 0;
	std::map<std::string, FMOD::Sound*>::iterator itr;

	SoundEvent* NextSound = NULL;
	while(!m_soundQueue.empty()) {
		NextSound = m_soundQueue.front();
		m_soundQueue.pop();

		// check if there is a channel left
		result = m_system->getChannelsPlaying(&channels);
		ERRCHECK(result);
		if (channels >= SOUND_MAX_CHANNELS-1 || result != FMOD_OK) {
			delete NextSound;
			break;
		}

		itr = m_sounds.find(NextSound->name);

		if (itr != m_sounds.end()) {
			FMOD::Channel* channel = NULL;

			result = m_system->playSound(FMOD_CHANNEL_FREE, itr->second, true, &channel);
			if (!ERRCHECK(result)) {
				delete NextSound;
				break;
			}

			result = channel->set3DAttributes(&NextSound->position, &NextSound->velocity);
			ERRCHECK(result);

			channel->setVolume(NextSound->volume);
			result = channel->setPaused(false);
			ERRCHECK(result);
		}
		delete NextSound;
	}

	// clean up if the exited the loop above
	while(!m_soundQueue.empty()) {
		NextSound = m_soundQueue.front();
		m_soundQueue.pop();
		delete NextSound;
	}

	if (m_musicEnabled) {
		bool playing = false;
		if (m_musicChannel)
			m_musicChannel->isPlaying(&playing);
		if (!playing) {
			PlayMusic(1);
		}
	}

	m_system->update();
}

void SoundMgr::SetListenerPos(float* listenerpos, float* forward, float* upward, float* velocity)
{
	FMOD_RESULT result;
	FMOD_VECTOR pos;
	pos.x = listenerpos[0] * 0.025f;
	pos.y = listenerpos[1] * 0.025f;
	pos.z = listenerpos[2] * 0.025f;
	FMOD_VECTOR dir;
	dir.x = -forward[0];
	dir.y = -forward[1];
	dir.z = -forward[2];
	FMOD_VECTOR up;
	up.x = upward[0];
	up.y = upward[1];
	up.z = upward[2];
	FMOD_VECTOR vel;
	vel.x = velocity[0];
	vel.y = velocity[1];
	vel.z = velocity[2];

	result = m_system->set3DListenerAttributes(0, &pos, &vel, &dir, &up);
	ERRCHECK(result);
}

unsigned SoundMgr::LoadSound(const std::string& folder)
{
	int count = 0;
	FMOD::Sound *sound;
	FMOD_RESULT result;
	using namespace boost::filesystem;

	path p(folder);

	if (is_directory(p) && !is_empty(p)) {
		directory_iterator end_itr;
		for (directory_iterator itr(p); itr != end_itr; ++itr) {
			if (itr->leaf().size() > 3) {
				count++;

				result = m_system->createSound(itr->string().c_str(), FMOD_HARDWARE | FMOD_3D, 0, &sound);
				ERRCHECK(result);

				result = sound->set3DMinMaxDistance(0.5f, 200000.0f);
				ERRCHECK(result);

				result = sound->setMode(FMOD_LOOP_OFF);
				ERRCHECK(result);

				m_sounds[basename(*itr)] = sound;
			}
		}
	}
	return count;
}

unsigned SoundMgr::LoadMusic(const std::string& folder)
{
	int count = 0;
	FMOD::Sound *sound;
	FMOD_RESULT result;
	using namespace boost::filesystem;

	path p(folder);

	if (is_directory(p) && !is_empty(p)) {
		directory_iterator end_itr;
		for (directory_iterator itr(p); itr != end_itr; ++itr) {
			if (itr->leaf().size() > 3) {
				count++;
				result = m_system->createSound(itr->string().c_str(), FMOD_HARDWARE | FMOD_CREATESTREAM, 0, &sound);
				ERRCHECK(result);

				result = sound->setMode(FMOD_LOOP_OFF);
				ERRCHECK(result);

				m_music[basename(*itr)] = sound;
			}
		}
	}
	m_currentMusic = m_music.begin();
	return count;
}

void SoundMgr::PlaySound(const std::string& name, float volume, float* position, float* velocity)
{
	SoundEvent* NewEvent = new SoundEvent();
	NewEvent->name = name;
	NewEvent->volume = volume;
	NewEvent->position.x = position[0] * 0.025f;
	NewEvent->position.y = position[1] * 0.025f;
	NewEvent->position.z = position[2] * 0.025f;
	NewEvent->velocity.x = velocity[0];
	NewEvent->velocity.y = velocity[1];
	NewEvent->velocity.z = velocity[2];

	m_mutex.lock();
	m_soundQueue.push(NewEvent);
	m_mutex.unlock();
}

void SoundMgr::PlayMusic(float volume)
{
	FMOD_RESULT result;

	if (m_currentMusic != m_music.end()) {
		m_currentMusic++;
		if (m_currentMusic == m_music.end())
			m_currentMusic = m_music.begin();
		result = m_system->playSound(FMOD_CHANNEL_FREE, m_currentMusic->second, true, &m_musicChannel);
		ERRCHECK(result);
		m_musicChannel->setVolume(volume);
		result = m_musicChannel->setPaused(false);
		ERRCHECK(result);
	}
}

}
