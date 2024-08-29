#include "SoundManager.h"

SoundManager::SoundManager() : m_currentTrack("")
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::loadSound(const std::string& m_name, const std::string& m_filename)
{
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(m_filename))
    {
        m_soundBuffers[m_name] = buffer;
        m_sounds[m_name].setBuffer(m_soundBuffers[m_name]);
    }
}

void SoundManager::playSound(const std::string& m_name)
{
    if (m_sounds.find(m_name) != m_sounds.end())
    {
        m_sounds[m_name].setVolume(m_globalSoundVolume);
        m_sounds[m_name].play();
    }
}

void SoundManager::stopSound(const std::string& m_name)
{
    if (m_sounds.find(m_name) != m_sounds.end())
    {
        m_sounds[m_name].stop();
    }
}

void SoundManager::setSoundVolume(const std::string& m_name, float m_volume)
{
    if (m_sounds.find(m_name) != m_sounds.end())
    {
        m_sounds[m_name].setVolume(m_volume);
    }
}

void SoundManager::setGlobalSoundVolume(float m_volume)
{
    m_globalSoundVolume = m_volume;
    for (auto& sound : m_sounds)
    {
        sound.second.setVolume(m_volume);
    }
}

void SoundManager::loadMusic(const std::string& m_name, const std::string& m_filename)
{
    sf::Music* music = new sf::Music();
    if (music->openFromFile(m_filename))
    {
        m_musics[m_name] = music;
    }
}

void SoundManager::playMusic(const std::string& m_name, bool m_loop)
{
    auto it = m_musics.find(m_name);
    if (it != m_musics.end())
    {
        it->second->setLoop(m_loop);
        it->second->setVolume(m_globalMusicVolume);
        it->second->play();
    }
}

void SoundManager::stopMusic(const std::string& m_name)
{
    if (m_musics.find(m_name) != m_musics.end())
    {
        m_musics[m_name]->stop();
    }
}

void SoundManager::setMusicVolume(const std::string& m_name, float m_volume)
{
    if (m_musics.find(m_name) != m_musics.end())
    {
        m_musics[m_name]->setVolume(m_volume);
    }
}

void SoundManager::setGlobalMusicVolume(float m_volume)
{
    m_globalMusicVolume = m_volume;
    for (auto& music : m_musics)
    {
        if (music.second != nullptr)
        {
            music.second->setVolume(m_volume);
        }
    }
}

void SoundManager::addToPlaylist(const std::string& m_name)
{
    if (m_musics.find(m_name) != m_musics.end())
    {
        m_playlist.push(m_name);
    }
}

void SoundManager::playNextTrack()
{
    if (!m_playlist.empty())
    {
        if (!m_currentTrack.empty() && m_musics[m_currentTrack]->getStatus() == sf::Music::Playing)
        {
            return;
        }

        m_currentTrack = m_playlist.front();
        m_playlist.pop();
        m_playlist.push(m_currentTrack);

        playMusic(m_currentTrack, false);
    }
}

/// <summary>
/// updates the track when the current one stops playing
/// </summary>
void SoundManager::updateMusicTrack()
{
    if (!m_currentTrack.empty() && m_musics[m_currentTrack]->getStatus() != sf::Music::Playing)
    {
        playNextTrack();
    }
}

void SoundManager::stopAllMusic()
{
    for (auto& music : m_musics)
    {
        music.second->stop();
    }
    m_currentTrack = "";
}

void SoundManager::loadAll()
{
    // Music
    loadMusic("MenuMusic", "Assets\\Audio\\background1.ogg");

    // Playlist musics
    loadMusic("gameplayMusic1", "Assets\\Audio\\temple.mp3");
    loadMusic("gameplayMusic2", "Assets\\Audio\\magical_theme.mp3");
    loadMusic("gameplayMusic3", "Assets\\Audio\\game_music.wav");
    addToPlaylist("gameplayMusic1");
    addToPlaylist("gameplayMusic2");
    addToPlaylist("gameplayMusic3");

    // Sounds
    // Menu sounds
    loadSound("buttonClick", "Assets\\Audio\\click07.mp3");

    // Enemy sounds
    loadSound("grenade", "Assets\\Audio\\grenade.ogg");

    // Player sounds
    loadSound("pickup_coin", "Assets\\Audio\\pickup_coin.wav");
    loadSound("jump", "Assets\\Audio\\jump.wav");
    loadSound("pickup", "Assets\\Audio\\pickup.wav");
    loadSound("swing1", "Assets\\Audio\\swing.wav");
    loadSound("swing2", "Assets\\Audio\\swing2.wav");

    // World/Trap sounds
    loadSound("platform", "Assets\\Audio\\platform_crumble.wav");
    loadSound("pickup_coin", "Assets\\Audio\\pickup_coin.wav");
    loadSound("level_complete", "Assets\\Audio\\level_complete.wav");
    loadSound("trapXplosion", "Assets\\Audio\\explosion07.wav");

}
