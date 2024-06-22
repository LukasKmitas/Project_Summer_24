#ifndef MAINMENU_H
#define MAINMENU_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <array>
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

    int getPressedItem() { return m_selectedItemIndex; }
    int handleClick(sf::Vector2f m_mousePos);
    
    void clearParticles();

private:

    ParticleManager m_particleManager;

    void setupMenu();

    int m_selectedItemIndex;

    sf::Font m_font;
    std::array<sf::Text, 4> m_menuText;
    std::array<sf::RectangleShape, 4> m_menuButton;

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

};

#endif
