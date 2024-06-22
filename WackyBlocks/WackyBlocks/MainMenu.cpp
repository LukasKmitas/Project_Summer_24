#include "MainMenu.h"

MainMenu::MainMenu() : m_selectedItemIndex(0)
{
    if (!m_font.loadFromFile("ASSETS/FONTS/ariblk.ttf"))
    {
        std::cout << "problem loading Font" << std::endl;
    }
    setupMenu();
    SoundManager::getInstance().loadAll();
    SoundManager::getInstance().playMusic("MenuMusic", true);

    m_particleManager.createBackgroundEffect();
}

MainMenu::~MainMenu()
{
    clearParticles();
}

void MainMenu::update(sf::Time t_deltaTime)
{
    m_particleManager.update(t_deltaTime, ParticleType::FIREFLY);
    m_particleManager.update(t_deltaTime, ParticleType::BOUNCY_BALL);
}

void MainMenu::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_backgroundSprite);

    for (int i = 0; i < m_menuText.size(); ++i)
    {
        m_window.draw(m_menuButton[i]);
        m_window.draw(m_menuText[i]);
    }

    m_particleManager.render(m_window, ParticleType::FIREFLY);
    m_particleManager.render(m_window, ParticleType::BOUNCY_BALL);

}

void MainMenu::handleMouseHover(sf::Vector2f m_mousePos)
{
    for (int i = 0; i < m_menuText.size(); ++i)
    {
        if (m_menuButton[i].getGlobalBounds().contains(m_mousePos))
        {
            m_menuText[i].setFillColor(sf::Color::Red);
        }
        else
        {
            m_menuText[i].setFillColor(sf::Color::White);
        }
    }
}

void MainMenu::handleRightClick(sf::Vector2f m_mousePos)
{
    m_particleManager.createBouncyBallEffect(m_mousePos);
}

int MainMenu::handleClick(sf::Vector2f m_mousePos)
{
    for (int i = 0; i < m_menuText.size(); ++i)
    {
        if (m_menuButton[i].getGlobalBounds().contains(m_mousePos))
        {
            return i;
        }
    }
    return -1;
}

void MainMenu::clearParticles()
{
    m_particleManager.clearAllParticles();
}

void MainMenu::moveUp()
{
    if (m_selectedItemIndex - 1 >= 0)
    {
        m_menuText[m_selectedItemIndex].setFillColor(sf::Color::White);
        m_selectedItemIndex--;
        m_menuText[m_selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

void MainMenu::moveDown()
{
    if (m_selectedItemIndex + 1 < m_menuText.size())
    {
        m_menuText[m_selectedItemIndex].setFillColor(sf::Color::White);
        m_selectedItemIndex++;
        m_menuText[m_selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

void MainMenu::setupMenu()
{
    std::array<std::string, 4> menuStrings = { "Play Game", "Level Editor", "Options", "Exit" };
    float buttonWidth = 200.0f;
    float buttonHeight = 50.0f;
    float outlineThickness = 2.0f;
    float xPosition = 50.0f;
    float yPosition = 50.0f;
    float xOffset = 0.0f;
    float yOffset = 80.0f;

    for (int i = 0; i < m_menuText.size(); ++i)
    {
        // Set up text
        m_menuText[i].setFont(m_font);
        m_menuText[i].setFillColor(sf::Color::White);
        m_menuText[i].setString(menuStrings[i]);
        m_menuText[i].setPosition(xPosition + outlineThickness, yPosition + i * yOffset + outlineThickness);

        // Set up button rect
        m_menuButton[i].setSize(sf::Vector2f(buttonWidth, buttonHeight));
        m_menuButton[i].setFillColor(sf::Color::Transparent);
        m_menuButton[i].setOutlineColor(sf::Color::Red);
        m_menuButton[i].setOutlineThickness(outlineThickness);
        m_menuButton[i].setPosition(xPosition, yPosition + i * yOffset);
    }

    m_menuText[0].setFillColor(sf::Color::Red);

    if (!m_backgroundTexture.loadFromFile("ASSETS\\IMAGES\\BG.png"))
    {
        std::cout << "problem loading Background Image" << std::endl;
    }
    m_backgroundSprite.setTexture(m_backgroundTexture);
    m_backgroundSprite.setScale
    (
        SCREEN_WIDTH / m_backgroundSprite.getLocalBounds().width,
        SCREEN_HEIGHT / m_backgroundSprite.getLocalBounds().height
    );
}
