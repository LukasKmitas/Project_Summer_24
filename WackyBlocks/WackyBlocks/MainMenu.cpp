#include "MainMenu.h"

MainMenu::MainMenu() : m_selectedItemIndex(0)
{
    if (!m_font.loadFromFile("ASSETS/FONTS/ariblk.ttf"))
    {
        std::cout << "problem loading Font" << std::endl;
    }
    setupMenu();
    setupSubmenu();
    setupLevelSelection();
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

    if (!m_showSubmenu && !m_showLevelSelection)
    {
        for (int i = 0; i < m_menuText.size(); ++i)
        {
            m_window.draw(m_menuButton[i]);
            m_window.draw(m_menuText[i]);
        }
    }
    else if (m_showSubmenu)
    {
        for (int i = 0; i < 2; ++i)
        {
            m_window.draw(m_submenuButton[i]);
            m_window.draw(m_submenuText[i]);
        }
    }
    else if (m_showLevelSelection)
    {
        m_window.draw(m_levelSelectionBox);
        for (int i = 0; i < m_levelButtons.size(); ++i)
        {
            m_window.draw(m_levelButtons[i]);
            m_window.draw(m_levelTexts[i]);
        }
        m_window.draw(m_continueButton);
        m_window.draw(m_continueButtonText);
    }

    m_particleManager.render(m_window, ParticleType::FIREFLY);
    m_particleManager.render(m_window, ParticleType::BOUNCY_BALL);

}

void MainMenu::handleMouseHover(sf::Vector2f m_mousePos)
{
    if (!m_showSubmenu && !m_showLevelSelection)
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
    else if (m_showSubmenu)
    {
        updateSubmenuButtonColors(m_mousePos);
    }
    else if (m_showLevelSelection)
    {
        for (int i = 0; i < m_levelButtons.size(); ++i)
        {
            if (m_levelButtons[i].getGlobalBounds().contains(m_mousePos))
            {
                m_levelTexts[i].setFillColor(sf::Color::Red);
            }
            else
            {
                m_levelTexts[i].setFillColor(sf::Color::White);
            }
        }

        if (m_continueButton.getGlobalBounds().contains(m_mousePos))
        {
            m_continueButtonText.setFillColor(sf::Color::Red);
        }
        else
        {
            m_continueButtonText.setFillColor(sf::Color::White);
        }
    }
}

void MainMenu::handleRightClick(sf::Vector2f m_mousePos)
{
    m_particleManager.createBouncyBallEffect(m_mousePos);
}

int MainMenu::handleClick(sf::Vector2f m_mousePos)
{
    if (!m_showSubmenu && !m_showLevelSelection)
    {
        for (int i = 0; i < m_menuText.size(); ++i)
        {
            if (m_menuButton[i].getGlobalBounds().contains(m_mousePos))
            {
                if (i == 0) // Play Game
                {
                    m_showLevelSelection = true;
                }
                return i;
            }
        }
    }
    else if (m_showSubmenu)
    {
        for (int i = 0; i < 2; ++i)
        {
            if (m_submenuButton[i].getGlobalBounds().contains(m_mousePos))
            {
                m_showSubmenu = false;
                return i + 4; // submenu options
            }
        }
    }
    else if (m_showLevelSelection)
    {
        for (int i = 0; i < m_levelButtons.size(); ++i)
        {
            if (m_levelButtons[i].getGlobalBounds().contains(m_mousePos))
            {
                m_selectedLevelIndex = i;
                for (auto& button : m_levelButtons)
                {
                    button.setFillColor(sf::Color::Transparent);
                }
                m_levelButtons[i].setFillColor(sf::Color::Green);
                updateContinueButtonColor();
            }
        }

        if (m_continueButton.getGlobalBounds().contains(m_mousePos))
        {
            if (m_selectedLevelIndex != -1)
            {
                m_showLevelSelection = false;
                m_showSubmenu = true;
                return 6;
            }
        }
    }
    return -1;
}

std::string MainMenu::getSelectedLevelFile()
{
    if (m_selectedLevelIndex != -1)
    {
        return m_levelTexts[m_selectedLevelIndex].getString();
    }
    return "";
}

void MainMenu::clearParticles()
{
    m_particleManager.clearAllParticles();
}

void MainMenu::showLevelSelection()
{
    m_showLevelSelection = true;
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

void MainMenu::setupSubmenu()
{
    std::array<std::string, 2> submenuStrings = { "Single Player", "Multiplayer" };
    float buttonWidth = 250.0f;
    float buttonHeight = 50.0f;
    float outlineThickness = 2.0f;
    float xPosition = 50.0f;
    float yPosition = 50.0f;
    float xOffset = 0.0f;
    float yOffset = 80.0f;

    for (int i = 0; i < 2; ++i)
    {
        m_submenuText[i].setFont(m_font);
        m_submenuText[i].setFillColor(sf::Color::White);
        m_submenuText[i].setString(submenuStrings[i]);
        m_submenuText[i].setPosition(xPosition + outlineThickness, yPosition + i * yOffset + outlineThickness);

        m_submenuButton[i].setSize(sf::Vector2f(buttonWidth, buttonHeight));
        m_submenuButton[i].setFillColor(sf::Color::Transparent);
        m_submenuButton[i].setOutlineColor(sf::Color::Red);
        m_submenuButton[i].setOutlineThickness(outlineThickness);
        m_submenuButton[i].setPosition(xPosition, yPosition + i * yOffset);
    }
}

void MainMenu::setupLevelSelection()
{
    float boxWidth = 600.0f;
    float boxHeight = 400.0f;
    float outlineThickness = 2.0f;
    float xPosition = SCREEN_WIDTH / 2 - boxWidth / 2;
    float yPosition = SCREEN_HEIGHT / 2 - boxHeight / 2;

    m_levelSelectionBox.setSize(sf::Vector2f(boxWidth, boxHeight));
    m_levelSelectionBox.setFillColor(sf::Color(50, 50, 50, 200));
    m_levelSelectionBox.setOutlineColor(sf::Color::White);
    m_levelSelectionBox.setOutlineThickness(outlineThickness);
    m_levelSelectionBox.setPosition(xPosition, yPosition);

    m_continueButton.setSize(sf::Vector2f(200.0f, 50.0f));
    m_continueButton.setFillColor(sf::Color::Transparent);
    m_continueButton.setOutlineColor(sf::Color::Red);
    m_continueButton.setOutlineThickness(outlineThickness);
    m_continueButton.setPosition(xPosition + boxWidth / 2 - 100.0f, yPosition + boxHeight + 20.0f);

    m_continueButtonText.setFont(m_font);
    m_continueButtonText.setString("Continue");
    m_continueButtonText.setFillColor(sf::Color::White);
    m_continueButtonText.setPosition(m_continueButton.getPosition().x + 20.0f, m_continueButton.getPosition().y + 10.0f);

    updateContinueButtonColor();
    loadLevelFiles();
}

void MainMenu::loadLevelFiles()
{
    m_levelTexts.clear();
    m_levelButtons.clear();

    float buttonWidth = 550.0f;
    float buttonHeight = 50.0f;
    float outlineThickness = 2.0f;
    float xPosition = m_levelSelectionBox.getPosition().x + 20.0f;
    float yPosition = m_levelSelectionBox.getPosition().y + 20.0f;
    float xOffset = 0.0f;
    float yOffset = 60.0f;

    for (const auto& entry : std::filesystem::directory_iterator("ASSETS\\SAVEFILES"))
    {
        if (entry.is_regular_file())
        {
            std::string fileName = entry.path().filename().string();

            sf::Text levelText;
            levelText.setFont(m_font);
            levelText.setFillColor(sf::Color::White);
            levelText.setString(fileName);
            levelText.setPosition(xPosition + outlineThickness, yPosition + m_levelTexts.size() * yOffset + outlineThickness);

            sf::RectangleShape levelButton;
            levelButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
            levelButton.setFillColor(sf::Color::Transparent);
            levelButton.setOutlineColor(sf::Color::Red);
            levelButton.setOutlineThickness(outlineThickness);
            levelButton.setPosition(xPosition, yPosition + m_levelButtons.size() * yOffset);

            m_levelTexts.push_back(levelText);
            m_levelButtons.push_back(levelButton);
        }
    }
}

void MainMenu::updateContinueButtonColor()
{
    if (m_selectedLevelIndex != -1)
    {
        m_continueButton.setOutlineColor(sf::Color::Green);
    }
    else
    {
        m_continueButton.setOutlineColor(sf::Color::Red);
    }
}

void MainMenu::updateSubmenuButtonColors(sf::Vector2f m_mousePos)
{
    for (int i = 0; i < 2; ++i)
    {
        if (m_submenuButton[i].getGlobalBounds().contains(m_mousePos))
        {
            m_submenuText[i].setFillColor(sf::Color::Red);
        }
        else
        {
            m_submenuText[i].setFillColor(sf::Color::White);
        }
    }
}
