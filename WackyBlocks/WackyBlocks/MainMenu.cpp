#include "MainMenu.h"

MainMenu::MainMenu() : m_selectedItemIndex(0)
{
    if (!m_font.loadFromFile("ASSETS/FONTS/ariblk.ttf"))
    {
        std::cout << "problem loading Font" << std::endl;
    }
    setupMenu();
}

MainMenu::~MainMenu()
{
}

void MainMenu::render(sf::RenderWindow& window)
{
    for (size_t i = 0; i < m_menuText.size(); ++i)
    {
        window.draw(m_menuButton[i]);
        window.draw(m_menuText[i]);
    }
}

void MainMenu::handleMouseHover(sf::Vector2f m_mousePos)
{
    for (size_t i = 0; i < m_menuText.size(); ++i)
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

int MainMenu::handleClick(sf::Vector2f m_mousePos)
{
    for (size_t i = 0; i < m_menuText.size(); ++i)
    {
        if (m_menuButton[i].getGlobalBounds().contains(m_mousePos))
        {
            return i;
        }
    }
    return -1;
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

    for (size_t i = 0; i < m_menuText.size(); ++i)
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
}
