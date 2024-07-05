#ifndef MAINMENU_H
#define MAINMENU_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <array>
#include <filesystem>
#include "SoundManager.h"
#include "ParticleManager.h"

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
    void updateSessionList(const std::string& m_sessions);

    int getPressedItem() { return m_selectedItemIndex; }
    int handleClick(sf::Vector2f m_mousePos);
    
    std::string getSelectedLevelFile();

private:

    ParticleManager m_particleManager;

    void setupMenu();
    void setupSubmenu();
    void setupLevelSelection();
    void setupMultiplayerMenu();
    void setupHostGameScreen();
    void setupJoinGameScreen();
    void loadLevelFiles();
    void updateContinueButtonColor();

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

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    // Multiplayer screen
    sf::RectangleShape m_hostButton;
    sf::RectangleShape m_joinButton;
    sf::Text m_hostButtonText;
    sf::Text m_joinButtonText;

    // Host game screen
    sf::RectangleShape m_hostContinueButton;
    sf::Text m_hostContinueButtonText;
    sf::Text m_waitingForPlayerText;

    // Join game screen
    sf::Text m_findingGameText;
    sf::RectangleShape m_sessionListBox;
    std::vector<sf::Text> m_sessionTexts;
    std::vector<sf::RectangleShape> m_sessionButtons;
    sf::RectangleShape m_refreshButton;
    sf::Text m_refreshButtonText;

    bool m_showSubmenu = false;
    bool m_showLevelSelection = false;
    bool m_showMultiplayerOptions = false;
    bool m_showHostGameScreen = false;
    bool m_showJoinGameScreen = false;

};

#endif
