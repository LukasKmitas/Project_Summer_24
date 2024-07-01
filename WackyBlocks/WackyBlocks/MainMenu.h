#ifndef MAINMENU_H
#define MAINMENU_H

#include <iostream>
#include <SFML/Graphics.hpp>
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

    void moveUp();
    void moveDown();
    void handleMouseHover(sf::Vector2f m_mousePos);
    void handleRightClick(sf::Vector2f m_mousePos);
    void clearParticles();
    void showLevelSelection();

    int getPressedItem() { return m_selectedItemIndex; }
    int handleClick(sf::Vector2f m_mousePos);
    
    std::string getSelectedLevelFile();

private:

    ParticleManager m_particleManager;

    void setupMenu();
    void setupSubmenu();
    void setupLevelSelection();
    void loadLevelFiles();
    void updateContinueButtonColor();
    void updateSubmenuButtonColors(sf::Vector2f m_mousePos);

    int m_selectedItemIndex;
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

    bool m_showSubmenu = false;
    bool m_showLevelSelection = false;

};

#endif
