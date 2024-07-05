#include "Options.h"

Options::Options() : m_draggingSound(false), m_draggingMusic(false)
{
    if (!m_font.loadFromFile("Assets\\Fonts\\ariblk.ttf"))
    {
        std::cout << "problem loading Font" << std::endl;
    }
    setupOptions();
}

Options::~Options()
{
    clearParticles();
}

void Options::update(sf::Time t_deltaTime)
{
    m_particleManager.update(t_deltaTime, ParticleType::BOUNCY_BALL);
}

void Options::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_soundText);
    m_window.draw(m_musicText);
    m_window.draw(m_soundSlider);
    m_window.draw(m_musicSlider);
    m_window.draw(m_soundSliderHandle);
    m_window.draw(m_musicSliderHandle);
    m_window.draw(m_backButton);
    m_window.draw(m_backButtonText);

    m_particleManager.render(m_window, ParticleType::BOUNCY_BALL);

}

void Options::handleMouseClick(sf::Vector2f m_mousePos)
{
    if (m_backButton.getGlobalBounds().contains(m_mousePos))
    {
        m_backButtonPressed = true;
        SoundManager::getInstance().playSound("buttonClick");
    }
    else if (m_soundSliderHandle.getGlobalBounds().contains(m_mousePos))
    {
        m_draggingSound = true;
    }
    else if (m_musicSliderHandle.getGlobalBounds().contains(m_mousePos))
    {
        m_draggingMusic = true;
    }
}

void Options::handleMouseHover(sf::Vector2f m_mousePos)
{
    if (m_draggingSound)
    {
        float newVolume = std::min(std::max(m_mousePos.x - m_soundSlider.getPosition().x, 0.0f), m_soundSlider.getSize().x);
        SoundManager::getInstance().setGlobalSoundVolume(newVolume / m_soundSlider.getSize().x * 100);
        updateSliderPosition(m_soundSliderHandle, SoundManager::getInstance().globalSoundVolume);
    }
    else if (m_draggingMusic)
    {
        float newVolume = std::min(std::max(m_mousePos.x - m_musicSlider.getPosition().x, 0.0f), m_musicSlider.getSize().x);
        SoundManager::getInstance().setGlobalMusicVolume(newVolume / m_musicSlider.getSize().x * 100);
        updateSliderPosition(m_musicSliderHandle, SoundManager::getInstance().globalMusicVolume);
    }

    if (m_backButton.getGlobalBounds().contains(m_mousePos))
    {
        m_backButtonHovered = true;
        m_backButton.setFillColor(sf::Color(100, 100, 100));  // Highlight color
    }
    else
    {
        m_backButtonHovered = false;
        m_backButton.setFillColor(sf::Color::Transparent);  // Default color
    }
}

void Options::handleMouseRelease()
{
    m_draggingSound = false;
    m_draggingMusic = false;
}

void Options::reset()
{
    m_backButtonPressed = false;
}

bool Options::isBackButtonPressed() const
{
    return m_backButtonPressed;
}

void Options::clearParticles()
{
    m_particleManager.clearAllParticles();
}

void Options::setupOptions()
{
    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;
    float sliderWidth = 200.0f;

    m_soundText.setFont(m_font);
    m_soundText.setString("Sound Volume");
    m_soundText.setFillColor(sf::Color::White);
    m_soundText.setPosition(centerX - m_soundText.getGlobalBounds().width / 2, centerY - 100.0f);

    m_musicText.setFont(m_font);
    m_musicText.setString("Music Volume");
    m_musicText.setFillColor(sf::Color::White);
    m_musicText.setPosition(centerX - m_musicText.getGlobalBounds().width / 2, centerY);

    m_soundSlider.setSize(sf::Vector2f(sliderWidth, 5.0f));
    m_soundSlider.setFillColor(sf::Color::White);
    m_soundSlider.setPosition(centerX - sliderWidth / 2, centerY - 50.0f);

    m_musicSlider.setSize(sf::Vector2f(sliderWidth, 5.0f));
    m_musicSlider.setFillColor(sf::Color::White);
    m_musicSlider.setPosition(centerX - sliderWidth / 2, centerY + 50.0f);

    m_soundSliderHandle.setSize(sf::Vector2f(10.0f, 20.0f));
    m_soundSliderHandle.setFillColor(sf::Color::Red);
    m_soundSliderHandle.setPosition(centerX - sliderWidth / 2, centerY - 55.0f);
    updateSliderPosition(m_soundSliderHandle, SoundManager::getInstance().globalSoundVolume);

    m_musicSliderHandle.setSize(sf::Vector2f(10.0f, 20.0f));
    m_musicSliderHandle.setFillColor(sf::Color::Red);
    m_musicSliderHandle.setPosition(centerX - sliderWidth / 2, centerY + 45.0f);
    updateSliderPosition(m_musicSliderHandle, SoundManager::getInstance().globalMusicVolume);

    // Back button
    m_backButton.setSize(sf::Vector2f(100.0f, 50.0f));
    m_backButton.setFillColor(sf::Color::Transparent);
    m_backButton.setOutlineColor(sf::Color::White);
    m_backButton.setOutlineThickness(2.0f);
    m_backButton.setPosition(10.0f, 10.0f);

    m_backButtonText.setFont(m_font);
    m_backButtonText.setString("Back");
    m_backButtonText.setFillColor(sf::Color::White);
    m_backButtonText.setPosition(15.0f, 15.0f);
}

void Options::updateSliderPosition(sf::RectangleShape& m_sliderHandle, float m_volume)
{
    float sliderWidth = m_soundSlider.getSize().x;
    m_sliderHandle.setPosition(m_soundSlider.getPosition().x + m_volume / 100.0f * sliderWidth - m_sliderHandle.getSize().x / 2, m_sliderHandle.getPosition().y);
}

void Options::handleRightClick(sf::Vector2f m_mousePos)
{
    m_particleManager.createBouncyBallEffect(m_mousePos);
}