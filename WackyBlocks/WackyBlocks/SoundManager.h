#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <queue>

class SoundManager
{
public:

    static SoundManager& getInstance()
    {
        static SoundManager instance;
        return instance;
    }

    // Sound only to play for a short amount of time like a ding etc.
    void loadSound(const std::string& m_name, const std::string& m_filename);
    void playSound(const std::string& m_name);
    void stopSound(const std::string& m_name);
    void setSoundVolume(const std::string& m_name, float m_volume);
    void setGlobalSoundVolume(float m_volume);

    // Music that plays for longer duration 
    void loadMusic(const std::string& m_name, const std::string& m_filename);
    void playMusic(const std::string& m_name, bool m_loop);
    void stopMusic(const std::string& m_name);
    void setMusicVolume(const std::string& m_name, float m_volume);
    void setGlobalMusicVolume(float m_volume);

    // Playlist music that plays forever and can have a playlist
    void addToPlaylist(const std::string& m_name);
    void playNextTrack();
    void updateMusicTrack();
    void stopAllMusic();

    void loadAll();

    float globalSoundVolume = 100.0f;
    float globalMusicVolume = 100.0f;

private:

    SoundManager();
    ~SoundManager();

    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::map<std::string, sf::Sound> sounds;
    std::map<std::string, sf::Music*> musics;
    std::queue<std::string> playlist;
    std::string currentTrack;

};
