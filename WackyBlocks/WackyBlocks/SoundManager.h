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

    SoundManager();
    ~SoundManager();

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

    // Playlist music that plays forever
    void addToPlaylist(const std::string& m_name);
    void playNextTrack();
    void updateMusicTrack();
    void stopAllMusic();

    void loadAll();

    float m_globalSoundVolume = 100.0f;
    float m_globalMusicVolume = 100.0f;

private:

    std::map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::map<std::string, sf::Sound> m_sounds;
    std::map<std::string, sf::Music*> m_musics;
    std::queue<std::string> m_playlist;
    std::string m_currentTrack;

};
