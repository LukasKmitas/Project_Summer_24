#ifndef OPTIONSMENU_H
#define OPTIONSMENU_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "SoundManager.h"
#include "Global.h"
#include "ParticleManager.h"

class Options
{
public:

    Options();
    ~Options();

    void update(sf::Time t_deltaTime);
    void render(sf::RenderWindow& m_window);

    void handleMouseClick(sf::Vector2f m_mousePos);
    void handleMouseHover(sf::Vector2f m_mousePos);
    void handleMouseRelease();
    void handleRightClick(sf::Vector2f m_mousePos);

    void reset();
    bool isBackButtonPressed() const;

    void clearParticles();

private:

    ParticleManager m_particleManager;

    void setupOptions();
    void updateSliderPosition(sf::RectangleShape& m_sliderHandle, float m_volume);

    sf::Font m_font;
    sf::Text m_soundText;
    sf::Text m_musicText;
    sf::Text m_backButtonText;
    sf::RectangleShape m_backButton;
    sf::RectangleShape m_soundSlider;
    sf::RectangleShape m_musicSlider;
    sf::RectangleShape m_soundSliderHandle;
    sf::RectangleShape m_musicSliderHandle;

    bool m_draggingSound = false;
    bool m_draggingMusic = false;
    bool m_backButtonPressed = false;
    bool m_backButtonHovered = false;

};

#endif
