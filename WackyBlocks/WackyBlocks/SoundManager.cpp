#include "SoundManager.h"

SoundManager::SoundManager() : currentTrack("")
{
}

SoundManager::~SoundManager()
{
}

/// <summary>
/// Loads the sound 
/// </summary>
/// <param name="name"></param>
/// <param name="filename"></param>
void SoundManager::loadSound(const std::string& m_name, const std::string& m_filename)
{
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(m_filename))
    {
        soundBuffers[m_name] = buffer;
        sounds[m_name].setBuffer(soundBuffers[m_name]);
    }
}

/// <summary>
/// plays the sound
/// </summary>
/// <param name="name"></param>
void SoundManager::playSound(const std::string& m_name)
{
    if (sounds.find(m_name) != sounds.end())
    {
        sounds[m_name].setVolume(globalSoundVolume);
        sounds[m_name].play();
    }
}

/// <summary>
/// to stop the sound playing
/// </summary>
/// <param name="name"></param>
void SoundManager::stopSound(const std::string& m_name)
{
    if (sounds.find(m_name) != sounds.end())
    {
        sounds[m_name].stop();
    }
}

/// <summary>
/// sets the volume
/// </summary>
/// <param name="name"></param>
/// <param name="volume"></param>
void SoundManager::setSoundVolume(const std::string& m_name, float m_volume)
{
    if (sounds.find(m_name) != sounds.end())
    {
        sounds[m_name].setVolume(m_volume);
    }
}

/// <summary>
/// stes all sounds volume
/// </summary>
/// <param name="volume"></param>
void SoundManager::setGlobalSoundVolume(float m_volume)
{
    globalSoundVolume = m_volume;
    for (auto& sound : sounds)
    {
        sound.second.setVolume(m_volume);
    }
}

/// <summary>
/// Loads music
/// </summary>
/// <param name="name"></param>
/// <param name="filename"></param>
void SoundManager::loadMusic(const std::string& m_name, const std::string& m_filename)
{
    sf::Music* music = new sf::Music();
    if (music->openFromFile(m_filename))
    {
        musics[m_name] = music;
    }
}

/// <summary>
/// plays music
/// </summary>
/// <param name="name"></param>
/// <param name="loop"></param>
void SoundManager::playMusic(const std::string& m_name, bool m_loop)
{
    auto it = musics.find(m_name);
    if (it != musics.end())
    {
        it->second->setLoop(m_loop);
        it->second->setVolume(globalMusicVolume);
        it->second->play();
    }
}

/// <summary>
/// stops music
/// </summary>
/// <param name="name"></param>
void SoundManager::stopMusic(const std::string& m_name)
{
    if (musics.find(m_name) != musics.end())
    {
        musics[m_name]->stop();
    }
}

/// <summary>
/// sets music volume
/// </summary>
/// <param name="name"></param>
/// <param name="volume"></param>
void SoundManager::setMusicVolume(const std::string& m_name, float m_volume)
{
    if (musics.find(m_name) != musics.end())
    {
        musics[m_name]->setVolume(m_volume);
    }
}

/// <summary>
/// sets all music volume
/// </summary>
/// <param name="volume"></param>
void SoundManager::setGlobalMusicVolume(float m_volume)
{
    globalMusicVolume = m_volume;
    for (auto& music : musics)
    {
        if (music.second != nullptr)
        {
            music.second->setVolume(m_volume);
        }
    }
}

/// <summary>
/// adds music to playlist 
/// </summary>
/// <param name="name"></param>
void SoundManager::addToPlaylist(const std::string& m_name)
{
    if (musics.find(m_name) != musics.end())
    {
        playlist.push(m_name);
    }
}

/// <summary>
/// plays the next track for music if there is any
/// </summary>
void SoundManager::playNextTrack()
{
    if (!playlist.empty())
    {
        if (!currentTrack.empty() && musics[currentTrack]->getStatus() == sf::Music::Playing)
        {
            return;
        }

        currentTrack = playlist.front();
        playlist.pop();
        playlist.push(currentTrack);

        playMusic(currentTrack, false);
    }
}

/// <summary>
/// updates the track when the current one stops playing
/// </summary>
void SoundManager::updateMusicTrack()
{
    if (!currentTrack.empty() && musics[currentTrack]->getStatus() != sf::Music::Playing)
    {
        playNextTrack();
    }
}

/// <summary>
/// stops the music
/// </summary>
void SoundManager::stopAllMusic()
{
    for (auto& music : musics)
    {
        music.second->stop();
    }
    currentTrack = "";
}

/// <summary>
/// Loads everything music and sound
/// </summary>
void SoundManager::loadAll()
{
    // Musics
    loadMusic("MenuMusic", "Assets\\Audio\\background1.ogg");
    
    // Sounds
    loadSound("buttonClick", "Assets\\Audio\\click07.mp3");
}
