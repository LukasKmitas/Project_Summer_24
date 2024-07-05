#include "MainMenu.h"

MainMenu::MainMenu()
{
    if (!m_font.loadFromFile("Assets\\Fonts\\ariblk.ttf"))
    {
        std::cout << "problem loading Font" << std::endl;
    }
    setupMenu();
    setupSubmenu();
    setupLevelSelection();
    setupMultiplayerMenu();
    setupHostGameScreen();
    setupJoinGameScreen();
    SoundManager::getInstance().loadAll();
    //SoundManager::getInstance().playMusic("MenuMusic", true);

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

    if (!m_showSubmenu && !m_showLevelSelection && !m_showMultiplayerOptions && !m_showHostGameScreen && !m_showJoinGameScreen)
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
    else if (m_showMultiplayerOptions)
    {
        m_window.draw(m_hostButton);
        m_window.draw(m_hostButtonText);
        m_window.draw(m_joinButton);
        m_window.draw(m_joinButtonText);
    }
    else if (m_showHostGameScreen)
    {
        m_window.draw(m_waitingForPlayerText);
        m_window.draw(m_hostContinueButton);
        m_window.draw(m_hostContinueButtonText);
    }
    else if (m_showJoinGameScreen)
    {
        m_window.draw(m_findingGameText);
        m_window.draw(m_refreshButton);
        m_window.draw(m_refreshButtonText);

        m_window.draw(m_sessionListBox);
        for (int i = 0; i < m_sessionButtons.size(); ++i)
        {
            m_window.draw(m_sessionButtons[i]);
            m_window.draw(m_sessionTexts[i]);
        }
    }

    m_particleManager.render(m_window, ParticleType::FIREFLY);
    m_particleManager.render(m_window, ParticleType::BOUNCY_BALL);

}

void MainMenu::handleMouseHover(sf::Vector2f m_mousePos)
{
    if (!m_showSubmenu && !m_showLevelSelection && !m_showMultiplayerOptions && !m_showHostGameScreen && !m_showJoinGameScreen)
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
    else if (m_showMultiplayerOptions)
    {
        if (m_hostButton.getGlobalBounds().contains(m_mousePos))
        {
            m_hostButtonText.setFillColor(sf::Color::Red);
        }
        else
        {
            m_hostButtonText.setFillColor(sf::Color::White);
        }

        if (m_joinButton.getGlobalBounds().contains(m_mousePos))
        {
            m_joinButtonText.setFillColor(sf::Color::Red);
        }
        else
        {
            m_joinButtonText.setFillColor(sf::Color::White);
        }
    }
    else if (m_showHostGameScreen)
    {
        if (m_hostContinueButton.getGlobalBounds().contains(m_mousePos))
        {
            m_hostContinueButtonText.setFillColor(sf::Color::Green);
        }
        else
        {
            m_hostContinueButtonText.setFillColor(sf::Color::Red);
        }
    }
    else if (m_showJoinGameScreen)
    {
        if (m_refreshButton.getGlobalBounds().contains(m_mousePos))
        {
            m_refreshButtonText.setFillColor(sf::Color::Green);
        }
        else
        {
            m_refreshButtonText.setFillColor(sf::Color::White);
        }
        for (int i = 0; i < m_sessionButtons.size(); ++i)
        {
            if (m_sessionButtons[i].getGlobalBounds().contains(m_mousePos))
            {
                m_sessionTexts[i].setFillColor(sf::Color::Red);
            }
            else
            {
                m_sessionTexts[i].setFillColor(sf::Color::White);
            }
        }
    }
}

void MainMenu::handleRightClick(sf::Vector2f m_mousePos)
{
    m_particleManager.createBouncyBallEffect(m_mousePos);
}

int MainMenu::handleClick(sf::Vector2f m_mousePos)
{
    if (!m_showSubmenu && !m_showLevelSelection && !m_showMultiplayerOptions && !m_showHostGameScreen && !m_showJoinGameScreen)
    {
        for (int i = 0; i < m_menuText.size(); ++i)
        {
            if (m_menuButton[i].getGlobalBounds().contains(m_mousePos))
            {
                if (i == 0) // Play Game
                {
                    m_showSubmenu = true;
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
                if (i == 0) // Single Player
                {
                    m_showLevelSelection = true;
                }
                else if (i == 1) // Multiplayer
                {
                    m_showMultiplayerOptions = true;
                }
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
                m_showSubmenu = false;
                return 6; // continue button clicked
            }
        }
    }
    else if (m_showMultiplayerOptions)
    {
        if (m_hostButton.getGlobalBounds().contains(m_mousePos))
        {
            m_showHostGameScreen = true;
            m_showMultiplayerOptions = false;
            return 7;
        }
        else if (m_joinButton.getGlobalBounds().contains(m_mousePos))
        {
            m_showJoinGameScreen = true;
            m_showMultiplayerOptions = false;
            return 8;
        }
    }
    else if (m_showHostGameScreen)
    {
        if (m_hostContinueButton.getGlobalBounds().contains(m_mousePos))
        {
            return 9;
        }
    }
    else if (m_showJoinGameScreen)
    {
        if (m_refreshButton.getGlobalBounds().contains(m_mousePos))
        {
            return 10;
        }
        for (int i = 0; i < m_sessionButtons.size(); ++i)
        {
            if (m_sessionButtons[i].getGlobalBounds().contains(m_mousePos))
            {
                std::cout << "Joining session: " << m_sessionTexts[i].getString().toAnsiString() << std::endl;
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

void MainMenu::showSubmenu()
{
    m_showSubmenu = true;
}

void MainMenu::showLevelSelection()
{
    m_showLevelSelection = true;
}

void MainMenu::showMultiplayerOptions()
{
    m_showMultiplayerOptions = true;
}

void MainMenu::showHostGameScreen()
{
    m_showHostGameScreen = true;
}

void MainMenu::showJoinGameScreen()
{
    m_showJoinGameScreen = true;
}

void MainMenu::updateSessionList(const std::string& m_sessions)
{
    m_sessionTexts.clear();
    m_sessionButtons.clear();

    std::istringstream stream(m_sessions);
    std::string session;
    float buttonWidth = 550.0f;
    float buttonHeight = 50.0f;
    float outlineThickness = 2.0f;
    float xPosition = m_sessionListBox.getPosition().x + 20.0f;
    float yPosition = m_sessionListBox.getPosition().y + 20.0f;
    float yOffset = 60.0f;

    int index = 0;
    while (std::getline(stream, session))
    {
        sf::Text sessionText;
        sessionText.setFont(m_font);
        sessionText.setFillColor(sf::Color::White);
        sessionText.setString("Session " + std::to_string(index + 1) + ": " + session);
        sessionText.setPosition(xPosition + outlineThickness, yPosition + index * yOffset + outlineThickness);

        sf::RectangleShape sessionButton;
        sessionButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        sessionButton.setFillColor(sf::Color::Transparent);
        sessionButton.setOutlineColor(sf::Color::Red);
        sessionButton.setOutlineThickness(outlineThickness);
        sessionButton.setPosition(xPosition, yPosition + index * yOffset);

        m_sessionTexts.push_back(sessionText);
        m_sessionButtons.push_back(sessionButton);

        ++index;
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

    if (!m_backgroundTexture.loadFromFile("Assets\\Images\\Menu\\BG.png"))
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

void MainMenu::setupMultiplayerMenu()
{
    m_hostButton.setSize(sf::Vector2f(200.0f, 50.0f));
    m_hostButton.setFillColor(sf::Color::Transparent);
    m_hostButton.setOutlineColor(sf::Color::Red);
    m_hostButton.setOutlineThickness(2.0f);
    m_hostButton.setPosition(50.0f, 400.0f);

    m_hostButtonText.setFont(m_font);
    m_hostButtonText.setString("Host Game");
    m_hostButtonText.setFillColor(sf::Color::White);
    m_hostButtonText.setPosition(m_hostButton.getPosition().x + 10.0f, m_hostButton.getPosition().y + 10.0f);

    m_joinButton.setSize(sf::Vector2f(200.0f, 50.0f));
    m_joinButton.setFillColor(sf::Color::Transparent);
    m_joinButton.setOutlineColor(sf::Color::Red);
    m_joinButton.setOutlineThickness(2.0f);
    m_joinButton.setPosition(50.0f, 500.0f);

    m_joinButtonText.setFont(m_font);
    m_joinButtonText.setString("Join Game");
    m_joinButtonText.setFillColor(sf::Color::White);
    m_joinButtonText.setPosition(m_joinButton.getPosition().x + 10.0f, m_joinButton.getPosition().y + 10.0f);
}

void MainMenu::setupHostGameScreen()
{
    m_waitingForPlayerText.setFont(m_font);
    m_waitingForPlayerText.setString("Waiting for a player...");
    m_waitingForPlayerText.setFillColor(sf::Color::White);
    m_waitingForPlayerText.setPosition(SCREEN_WIDTH / 2 - m_waitingForPlayerText.getGlobalBounds().width / 2, 100);

    m_hostContinueButton.setSize(sf::Vector2f(200.0f, 50.0f));
    m_hostContinueButton.setFillColor(sf::Color::Transparent);
    m_hostContinueButton.setOutlineColor(sf::Color::Red);
    m_hostContinueButton.setOutlineThickness(2.0f);
    m_hostContinueButton.setPosition(SCREEN_WIDTH / 2 - 100.0f, 500.0f);

    m_hostContinueButtonText.setFont(m_font);
    m_hostContinueButtonText.setString("Continue");
    m_hostContinueButtonText.setFillColor(sf::Color::Red);
    m_hostContinueButtonText.setPosition(m_hostContinueButton.getPosition().x + 20.0f, m_hostContinueButton.getPosition().y + 10.0f);
}

void MainMenu::setupJoinGameScreen()
{
    float boxWidth = 600.0f;
    float boxHeight = 400.0f;
    float outlineThickness = 2.0f;
    float xPosition = SCREEN_WIDTH / 2 - boxWidth / 2;
    float yPosition = SCREEN_HEIGHT / 2 - boxHeight / 2;

    m_findingGameText.setFont(m_font);
    m_findingGameText.setString("Finding game...");
    m_findingGameText.setFillColor(sf::Color::White);
    m_findingGameText.setPosition(SCREEN_WIDTH / 2 - m_findingGameText.getGlobalBounds().width / 2, 100);

    m_sessionListBox.setSize(sf::Vector2f(boxWidth, boxHeight));
    m_sessionListBox.setFillColor(sf::Color(50, 50, 50, 200));
    m_sessionListBox.setOutlineColor(sf::Color::White);
    m_sessionListBox.setOutlineThickness(outlineThickness);
    m_sessionListBox.setPosition(xPosition, yPosition);

    m_refreshButton.setSize(sf::Vector2f(150.0f, 40.0f));
    m_refreshButton.setFillColor(sf::Color::Transparent);
    m_refreshButton.setOutlineColor(sf::Color::Red);
    m_refreshButton.setOutlineThickness(2.0f);
    m_refreshButton.setPosition(SCREEN_WIDTH / 2 - 75.0f, SCREEN_HEIGHT / 2 + 220.0f);

    m_refreshButtonText.setFont(m_font);
    m_refreshButtonText.setString("Refresh");
    m_refreshButtonText.setFillColor(sf::Color::White);
    m_refreshButtonText.setPosition(m_refreshButton.getPosition().x + 20.0f, m_refreshButton.getPosition().y + 5.0f);
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

    for (const auto& entry : std::filesystem::directory_iterator("Assets\\SaveFiles"))
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

