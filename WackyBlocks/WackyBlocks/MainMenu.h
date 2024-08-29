#ifndef MAINMENU_H
#define MAINMENU_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <array>
#include <filesystem>
#include <mutex>
#include "SoundManager.h"
#include "ParticleManager.h"

struct SessionInfo 
{
    sf::Text m_text;
    sf::RectangleShape m_button;
};

enum class ScreenState 
{
    MAIN_MENU,
    SUBMENU,
    LEVEL_SELECTION,
    MULTIPLAYER_OPTIONS,
    HOST_GAME_SCREEN,
    JOIN_GAME_SCREEN
};

class MainMenu
{
public:

    MainMenu();
    ~MainMenu();

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window);

    void handleMouseHover(sf::Vector2f m_mousePos);
    void handleRightClick(sf::Vector2f m_mousePos);
    void clearParticles();
    void showSubmenu();
    void showLevelSelection();
    void showMultiplayerOptions();
    void showHostGameScreen();
    void showJoinGameScreen();
    void showWaitingForHostScreen();
    void updateSessionList(const std::string& m_sessions);
    void setClientID(const std::string& m_id);
    void setPlayerJoined(bool m_joined);
    void loadLevelFiles();

    void resetMainMenu();

    int getPressedItem() { return m_selectedItemIndex; }
    int handleClick(sf::Vector2f m_mousePos);
    
    std::string getSelectedLevelFile();
    std::string getSelectedSession();
    std::string getClientID() const;

private:

    ParticleManager m_particleManager;
    ScreenState m_currentScreenState = ScreenState::MAIN_MENU;

    void setupMenu();
    void setupSubmenu();
    void setupLevelSelection();
    void setupMultiplayerMenu();
    void setupHostGameScreen();
    void setupJoinGameScreen();
    void updateContinueButtonColor();
    void setupBackButton();

    int m_selectedItemIndex = 0;
    int m_selectedLevelIndex = -1;

    sf::Font m_font;
    std::array<sf::Text, 4> m_menuText;
    std::array<sf::RectangleShape, 4> m_menuButton;
    std::vector<sf::Text> m_levelTexts;
    std::vector<sf::RectangleShape> m_levelButtons;
    sf::RectangleShape m_submenuButton[2];
    sf::RectangleShape m_levelSelectionBox;
    sf::RectangleShape m_continueButton;
    sf::Text m_continueButtonText;
    sf::Text m_submenuText[2];

    sf::RectangleShape m_backButton;
    sf::Text m_backButtonText;

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    // Multiplayer screen
    sf::RectangleShape m_hostButton;
    sf::RectangleShape m_joinButton;
    sf::Text m_hostButtonText;
    sf::Text m_joinButtonText;
    sf::Text m_clientIDText;
    std::string m_clientID;

    // Host game screen
    sf::RectangleShape m_hostContinueButton;
    sf::Text m_hostContinueButtonText;
    sf::Text m_waitingForPlayerText;

    // Join game screen
    sf::Text m_findingGameText;
    sf::RectangleShape m_sessionListBox;
    std::vector<SessionInfo> m_sessionsInfo;
    sf::RectangleShape m_refreshButton;
    sf::Text m_refreshButtonText;
    sf::Text m_waitingForHostText;

    std::mutex m_sessionMutex;

    bool m_showSubmenu = false;
    bool m_showLevelSelection = false;
    bool m_showMultiplayerOptions = false;
    bool m_showHostGameScreen = false;
    bool m_showJoinGameScreen = false;
    bool m_isPlayerJoined = false;
    bool m_showWaitingForHost = false;

};

#endif
